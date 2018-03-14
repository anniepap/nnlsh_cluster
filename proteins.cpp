#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include "curve.h"
#include "curveList.h"
#include "limits.h"
#include "hashTable.h"
#include "clusterInfo.h"
#include "initialization.h"
#include "assignment.h"
#include "update.h"
#include "evaluation.h"
#include "distanceFunctions.h"

#define number_of_clusters 4

using namespace std;

int main(int argc, char* argv[]) {

	int i, j, numConform, N;
	string in, line;
	string d = "";

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-i") == 0)
			in = argv[i+1];
	}
	if (in.empty()) {
		cout << "Please enter input file name: ";
		cin >> in;
	}
	
	//create empty curve list for input curves
	List *curveList = new List();

	//get all info from input file
	ifstream infile(in.c_str());
	if (infile.is_open()) {
		getline(infile, line);
		numConform = atoi(line.c_str());
		getline(infile, line);
		N = atoi(line.c_str());

		int count = 0;
		int curveId = 1;
		string points = "";
		while (getline(infile, line)) {
			points += line + "\t";
			count++;
			if (count == N) {
				//create curve struct and insert in curveList
				stringstream id;
				id << curveId;
			    Curve *newCurve = new Curve(id.str(), N, points, 3, 10000, -1, false);
			    curveList->ListInsert(*newCurve);
			    curveId++;
				count = 0;
				points = "";
			}
		}
		infile.close();
	}
	else {
		cout << "Unable to read input file." << endl;
		return -1;
	}

	for (i = 0; i < 2; i++) {
		//initialization
		List *centers = RandomSelection(number_of_clusters, numConform, curveList);

		//data structure to store clusters' info
		ClusterInfo** clusters = new ClusterInfo*[number_of_clusters];
		for (int k = 0; k < number_of_clusters; k++) {
			clusters[k] = new ClusterInfo();
			clusters[k]->setCentroid(centers->iterate(k));
		}

		vector<Curve*> centersVector;
		vector<Curve*> prevCentersVector;
		Node* temp = centers->getHead();
		while (temp != NULL) {
			centersVector.push_back(temp->info);
			temp = temp->next;
		}

		//while centroids are changing
		while (!centroidsAreEqual(centersVector, prevCentersVector)) {
			prevCentersVector.clear();
			for (j = 0; j < centersVector.size(); j++)
				prevCentersVector.push_back(centersVector[j]);

			//assignment
			clusters = LloydsAssignment(curveList, number_of_clusters, d, clusters, i);
			//update
			for (j = 0; j < number_of_clusters; j++)
				improvedUpdatePAM(clusters[j], d, i);

			centersVector.clear();
			for (j = 0; j < number_of_clusters; j++)
				centersVector.push_back(clusters[j]->getCentroid());
		}

		//print results in output file
		ofstream outfile;
		if (i == 0)
			outfile.open("crmsd.dat");
		else
			outfile.open("frechet.dat");
		if (!outfile.is_open()) {
			cout << "Unable to open output file." << endl;
			return -1;
		}

		outfile << "k: " << number_of_clusters << endl;
		outfile << "s: " << Silhouette(clusters, d, number_of_clusters, i) << endl;
		for (int k = 0; k < number_of_clusters; k++)
			outfile << endl << "Cluster_" << k+1 << ": " << clusters[k]->getCurves()->getCurveIds() << endl;
		outfile.close();

		//disallocate memory
		centers->deleteList();
		delete centers;
		for (j = 0; j < number_of_clusters; j++)
			delete clusters[j];
		delete[] clusters;
	}

	curveList->cleanInfo();
	delete curveList;

	return 0;
}
