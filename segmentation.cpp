#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include "curve.h"
#include "distanceFunctions.h"
using namespace std;

struct Coord {
	double lat;
	double lon;

	Coord(double lat, double lon) {
		this->lat = lat;
		this->lon = lon;
	}
};

bool find(vector<Coord>& crossroads, double lat, double lon) {
	for (int i = 0 ; i < crossroads.size() ; i++)
		if (crossroads[i].lat == lat && crossroads[i].lon == lon) return true;
	return false;
}

double getRadius(CurvePoint a, CurvePoint b, CurvePoint c) {
	if ((a.p[0] == b.p[0] && a.p[0] == c.p[0]) || (a.p[1] == b.p[1] && a.p[1] == c.p[1]))
		return 0;

	double dA = a.p[0]*a.p[0] + a.p[1]*a.p[1];
	double dB = b.p[0]*b.p[0] + b.p[1]*b.p[1];
	double dC = c.p[0]*c.p[0] + c.p[1]*c.p[1];

	double x = (dA*(c.p[1] - b.p[1]) + dB*(a.p[1] - c.p[1]) + dC*(b.p[1] - a.p[1]));
	double y = -1*(dA*(c.p[0] - b.p[0]) + dB*(a.p[0] - c.p[0]) + dC*(b.p[0] - a.p[0]));
	double div = (2*(a.p[0]*(c.p[1] - b.p[1]) + b.p[0]*(a.p[1] - c.p[1]) + c.p[0]*(b.p[1] - a.p[1])));

	//cout.precision(14);
	if (div == 0)
		return 0;

	CurvePoint center;
	center.dim = 2;
	center.p[0] = x/div;
	center.p[1] = y/div;
	// cout << "center: " << center.p[0] << ", " << center.p[1] << endl;

	return EuclideanDistance(center, a);
	
}

vector<CurvePoint> getCurve(istringstream& lineStream) {
	double lon, lat;
	char c;
	vector<CurvePoint> segmentNodes;
	while (!lineStream.eof()) {
		lineStream >> lat >> c >> lon >> c;
		CurvePoint node(lat,lon);
		segmentNodes.push_back(node);
	}
	return segmentNodes;
}

vector<double> getCurvature(vector<CurvePoint> segmentNodes) {
	double radius, prevRadius = 1000000;
	vector<double> lineCurvatures;

	if (segmentNodes.size() < 3) {
		lineCurvatures.push_back(-1);	// less than 3 points can't form triangle
		return lineCurvatures;
	} else {
		for (int i = 1; i < segmentNodes.size() - 1; i++) {
			//cout << endl;
			//cout << segmentNodes[i-1].p[0] << "," << segmentNodes[i-1].p[1] << " " << segmentNodes[i].p[0] << "," << segmentNodes[i].p[1] << " " << segmentNodes[i+1].p[0]<< "," << segmentNodes[i+1].p[1] << endl;
			radius = getRadius(segmentNodes[i-1],segmentNodes[i],segmentNodes[i+1]);
			//cout << " RADIUS " << radius << endl;
			if (i == 1)
				lineCurvatures.push_back(radius);
			else {
				if (radius < prevRadius)
					lineCurvatures.push_back(radius);
				else
					lineCurvatures.push_back(prevRadius);
			}
			prevRadius = radius;
		}
		lineCurvatures.push_back(radius);
	}
	// for (int i = 0; i < lineCurvatures.size(); i++)	{
	// 	cout << lineCurvatures[i] << endl;
	// }
	return lineCurvatures;
}

void printSegment(ofstream& fpFinalSegm, vector<CurvePoint> segmentNodesCoord, int segmentNumber, int wayId) {
	fpFinalSegm << segmentNumber << ", " << wayId << ", " << segmentNodesCoord.size();
	for (int k = 0 ; k < segmentNodesCoord.size() ; k++)
		fpFinalSegm << ", " << segmentNodesCoord[k].p[0] << ", " << segmentNodesCoord[k].p[1];
	fpFinalSegm << endl;
}

int main (int argc, char *argv[]) {
	vector<Coord> crossroads;

	ifstream fpWays(argv[1]);
	ifstream fpCrossroads(argv[2]);
	string tempSegmentFile(argv[3]);
	tempSegmentFile = "tmp_" + tempSegmentFile;
	ofstream fpSegment(tempSegmentFile.c_str());

	int nodeId;
	double lat, lon;
	char c;
	// store data of crossroads
	while (!fpCrossroads.eof()) {
		fpCrossroads >> nodeId >> c >> lat >> c >> lon;
		if (!fpCrossroads.good()) break;
		Coord node(lat, lon);
		crossroads.push_back(node);
	}

	int wayId, segmentNumber = 0;
	string type;
	while (!fpWays.eof()) {
		fpWays >> wayId;
		if (!fpWays.good()) break;
		fpWays.ignore(2);
		getline(fpWays, type, ',');
		 
		bool prevIsCrossroad = false; 
		vector<Coord> segmentNodes;
		string line;
		getline(fpWays, line);
		istringstream lineStream(line);
		while (!lineStream.eof()) {
			lineStream >> lat >> c >> lon >> c;
			bool isCrossroad = find(crossroads, lat, lon);
			// Previous segment ended
			if (prevIsCrossroad && !isCrossroad) {
				fpSegment << segmentNumber << ", " << wayId << ", " << (segmentNodes.size());
				for (int i = 0 ; i < segmentNodes.size(); i++)
					fpSegment << ", " << segmentNodes[i].lat << ", " << segmentNodes[i].lon;
				fpSegment << endl;
				segmentNumber++;
				segmentNodes.clear();
			}

			Coord node(lat, lon);
			segmentNodes.push_back(node);
			prevIsCrossroad = isCrossroad;
		}
		if (!segmentNodes.empty()) {
			fpSegment << segmentNumber << ", " << wayId << ", " << (segmentNodes.size());
			for (int i = 0 ; i < segmentNodes.size(); i++)
				fpSegment << ", " << segmentNodes[i].lat << ", " << segmentNodes[i].lon;
			fpSegment << endl;
			segmentNumber++;
		}
	}

	fpSegment.close();

	// Calculate curvature threshold
	ifstream fpSegm(tempSegmentFile.c_str());
	string line;
	vector<double> lineCurvatures;
	double sum = 0;	double threshold;
	int totalCurvatures = 0;
	while (!fpSegm.eof()) {
		getline(fpSegm, line);
		if(!fpSegm.good()) break;
		istringstream lineStream(line);
		int no, wayId, numOfNodes;
		lineStream >> no >> c >> wayId >> c >> numOfNodes >> c;
		vector<CurvePoint> segmentNodes = getCurve(lineStream);
		lineCurvatures = getCurvature(segmentNodes);

		if (lineCurvatures[0] != -1) {
			// cout << lineCurvatures.size() << endl;
			//totalCurvatures += lineCurvatures.size();
			for (int i = 1; i < lineCurvatures.size(); i++)	{
				double deltaCurvature = abs(lineCurvatures[i] - lineCurvatures[i-1]);				
				if (deltaCurvature == 0 || deltaCurvature > 1) continue;
				//cout << deltaCurvature << endl;
				//cout.precision(24);
				//cout << deltaCurvature << endl;
				sum += deltaCurvature;
				totalCurvatures++;
				// cout << "line " << lineCurvatures[i] << " ";
			}
			// cout << endl;
		}
	}
	threshold = sum / totalCurvatures;
	cout << "SUM " << sum << " TOTALcutvatures " << totalCurvatures << endl;
	cout << "THRESHOLD - mean of all curvatures: " << threshold << endl;
	// End of calculating threshold

	ofstream fpFinalSegm(argv[3]);
	fpSegm.clear();
	fpSegm.seekg(0, ios::beg);
	segmentNumber = 0;
	while (!fpSegm.eof()) {
		getline(fpSegm, line);
		if(!fpSegm.good()) break;
		istringstream lineStream(line);
		int no, wayId, numOfNodes;
		lineStream >> no >> c >> wayId >> c >> numOfNodes >> c;
		vector<CurvePoint> segmentNodes = getCurve(lineStream);
		lineCurvatures = getCurvature(segmentNodes);

		if (lineCurvatures[0] != -1) {
			vector<CurvePoint> segmentNodesCoord;
			segmentNodesCoord.push_back(segmentNodes[0]);
			segmentNodesCoord.push_back(segmentNodes[1]);
			int j = 2;
			for (int i = 1 ; i < lineCurvatures.size() ; i++) {
				double deltaCurvature = abs(lineCurvatures[i] - lineCurvatures[i-1]);

				if ((deltaCurvature > threshold && segmentNodesCoord.size() >= 5) ||
					(segmentNodesCoord.size() > 9)) {
					printSegment(fpFinalSegm, segmentNodesCoord, segmentNumber++, wayId);
					segmentNodesCoord.clear();
				}

				segmentNodesCoord.push_back(segmentNodes[j]);
				j++;
			}
			printSegment(fpFinalSegm, segmentNodesCoord, segmentNumber++, wayId);

		} else {
			printSegment(fpFinalSegm, segmentNodes, segmentNumber++, wayId);
		}
	}
	remove("tmp_segment.csv");

	return 0;
}
