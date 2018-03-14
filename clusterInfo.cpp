#include <iostream>
#include <string>
#include <vector>
#include "clusterInfo.h"
#include "curveList.h"
#include "curve.h"

using namespace std;

ClusterInfo::ClusterInfo() {
	centroid = NULL;
	curves = new List();
}

ClusterInfo::~ClusterInfo() {
	curves->deleteList();
	delete curves;
}

Curve* ClusterInfo::getCentroid() {
	return centroid;
}

List* ClusterInfo::getCurves() {
	return curves;
}

void ClusterInfo::setCentroid(Curve* c) {
	centroid = c;
}

void ClusterInfo::setCurves(List* curves) {
	this->curves = curves;
}

void ClusterInfo::setCurve(Curve* curve, int i) {
	curves->setCurve(curve, i);
}

void ClusterInfo::InsertCurve(Curve* curve, double dist, int center) {
	curve->setDist(dist);
	curve->setCenterId(center);
	curves->SortingInsert(*curve);
}

void ClusterInfo::printCluster() {
	cout << "center is " << centroid->getCurveId() << endl;
	// cout << "curves: ";
	// cout << curves->getCurveIds() << endl;
}

int ClusterInfo::getNumOfCurves() {
	return curves->numOfNodes;
}

bool centroidsAreEqual(vector<Curve*> centroidsA, vector<Curve*> centroidsB) {
	if (centroidsA.size() == 0 || centroidsB.size() == 0)
		return false;

	for (int i = 0; i < centroidsA.size(); i++) {
		if (centroidsA[i]->getCurveId() != centroidsB[i]->getCurveId())
			return false;
		else if (centroidsA[i]->getCurveId() == "MEANCURVE" && centroidsB[i]->getCurveId() == "MEANCURVE")
			return centroidsA[i]->comparePoints(centroidsB[i]);
	}
	return true;
}
