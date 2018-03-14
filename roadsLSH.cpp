#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <time.h>
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

#define number_of_clusters 12
#define number_of_grid_tables 4
#define number_of_hash_tables 1

using namespace std;

int main(int argc, char* argv[]) {

	int i, j, n, min, max, flag;
	char *dup, *tok;
	string in, d, line;
	clock_t start, stop;
	unsigned int seed = time(NULL);
	d = "DFT";
	flag = 1;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-i") == 0)
			in = argv[i+1];
	}
	srand(seed);

	if (in.empty()) {
		cout << "Please enter input file name: ";
		cin >> in;
	}

	//create empty curve list for input curves
	List *curveList = new List();
	n = 0;
	max = 0;
	min = 1000000;
	Limits limits;
	limits.minPoint = CurvePoint::initMin(2);
	limits.maxPoint = CurvePoint::initMax(2);

	//get all info from input file
	ifstream infile(in.c_str());
	if (infile.is_open()) {
		string curveId;
		while (getline(infile, curveId, ',')) {
			getline(infile, line);
			n++;

			char chars[] = " ";
			for (j = 0; j < strlen(chars); ++j)
				line.erase(remove(line.begin(), line.end(), chars[j]), line.end());

			dup = strdup(line.c_str());
			tok = strtok(dup, ",");

			//get number of points
			tok = strtok(NULL, ",");
			int m = atoi(tok);

			//get all points
			tok = strtok(NULL, "\n");
			string points(tok);

			//create curve struct and insert in curveList
		    Curve *newCurve = new Curve(curveId, m, points, 2, 10000, -1, false);
		    curveList->ListInsert(*newCurve);
			//get limits to use for creating the grid limits
			limits = GetLimits(limits, newCurve);
			free(dup);
		}
		infile.close();
	}
	else {
		cout << "Unable to read input file." << endl;
		return -1;
	}

	//store input curves in lsh tables
	GridHash* gridHash;
	gridHash = CreateLSHTables(2, n, min, max, number_of_grid_tables, number_of_hash_tables, number_of_clusters, limits, curveList);

	start = clock();
	//initialization
	List *centers = RandomSelection(number_of_clusters, n, curveList);

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
		clusters = ANN(gridHash, clusters, curveList, centers, d, number_of_clusters, number_of_hash_tables, number_of_grid_tables, 2, max, n);
		//update
		for (j = 0; j < number_of_clusters; j++)
			improvedUpdatePAM(clusters[j], d, flag);

		centersVector.clear();
		for (j = 0; j < number_of_clusters; j++)
			centersVector.push_back(clusters[j]->getCentroid());
	}
	stop = clock();
	double total = (double)(stop-start) / (CLOCKS_PER_SEC/1000);

	//print results in output file
	ofstream outfile;
	outfile.open("lsh_ways_clustering.dat");
	if (!outfile.is_open()) {
		cout << "Unable to open output file." << endl;
		return -1;
	}

	outfile << "k: " << number_of_clusters << endl;
	outfile << "s: " << Silhouette(clusters, d, number_of_clusters, flag) << endl;
	outfile <<  endl << "Clustering time: " << total << " msec" << endl;
	for (int k = 0; k < number_of_clusters; k++)
		outfile << endl << "Cluster_" << k+1 << ": " << clusters[k]->getCurves()->getCurveIds() << endl;
	outfile.close();

	//disallocate memory
	centers->deleteList();
	curveList->cleanInfo();
	delete centers;
	for (j = 0; j < number_of_clusters; j++)
		delete clusters[j];
	delete[] clusters;
	if (gridHash != NULL) {
		for (int l = 0; l < number_of_hash_tables; l++) {
			for (int k = 0; k < number_of_grid_tables; k++)
				delete gridHash[l].grids[k];
			delete[] gridHash[l].grids;
			delete gridHash[l].hashTable;
		}
		delete[] gridHash;
	}
	delete curveList;

	return 0;
}
