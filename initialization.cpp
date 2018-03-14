#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <cstdlib>
#include "curve.h"
#include "curveList.h"
#include "distanceFunctions.h"
#include "initialization.h"

using namespace std;

List* RandomSelection(int k, int N, List *curveList) {
	cout << endl << "RandomSelection" << endl;
	int id;
	Curve* curve;
	List* kCenters = new List();

	//set k random curves as centroids
	for (int i = 0; i < k; i++) {
		//pick a random curve from the dataset
		id = (N+1) * (rand() / (RAND_MAX + 1.0));
		curve = curveList->iterate(id);
		//if we haven't picked that one already add it to the list of centroids
		while (kCenters->CheckIfExists(curve)) {
			id = N * (rand() / (RAND_MAX + 1.0));
			curve = curveList->iterate(id);
		}
		kCenters->ListInsert(*curve);
		cout << "center " << i+1 << ": " << curve->getCurveId() << endl;  
	}
	return kCenters;
}

List* kMeansInit(int k, int totalCurves, List *curveList, string function) {

	cout << endl << "k-means++" << endl;
	int idC1 = totalCurves * (rand() / (RAND_MAX + 1.0));
	Curve* curve = curveList->getCurve(idC1);
	List* kCenters = new List(); 						// delete
	kCenters->ListInsert(*curve);
	int t = 1; 			//counter of current centroids

	while(t != k && t <= totalCurves) {
		double sumDists = 0.0;
		double* partialSums = new double[totalCurves-t+1]; // one extra element, so P(0) == 0
		partialSums[0] = 0.0;
		int partialSumsIndex = 1;
		int* partialSumsToCurveIndex = new int[totalCurves-t+1];
		partialSumsToCurveIndex[0] = -1;

		int i = 0;
		Node* tp = curveList->getHead();
		while (tp != NULL) {
		//for (int i = 0; i < totalCurves; i++) {
			Curve* curCurve = tp->info; //curveList->iterate(i);
			if (!kCenters->CheckIfExists(curCurve)) { // if not center
				double minDist = 1000000;  // D(i)
				double newDist;
				double maxDist = 0.0;
				for (int j = 0; j < t; j++) {
					Curve* curCenter = kCenters->iterate(j);
					if (function.compare("DFT") == 0)
						newDist = FrechetDistance(curCurve, curCenter);
					else if (function.compare("DTW") == 0)
						newDist = WarpingDistance(curCurve, curCenter);

					if (newDist < minDist)
						minDist = newDist;
					if (newDist > maxDist)
						maxDist = newDist;
				}
				newDist = minDist / maxDist; // normalize wth max_i D(i)
				double temp = sumDists + (newDist * newDist);
				partialSums[partialSumsIndex] = temp;
				partialSumsToCurveIndex[partialSumsIndex] = i;
				sumDists = temp;
				partialSumsIndex++;
			}
			i++;
			tp = tp->next;
		}
		// Pick a uniformly distributed float x in [0, P(totalCurves − t)]
		double x = partialSums[totalCurves-t] * (rand() / (RAND_MAX + 1.0));
		for (int j = 1; j <= totalCurves-t; j++) {
			if (partialSums[j-1] < x && x <= partialSums[j]) {
				kCenters->ListInsert(*curveList->iterate(partialSumsToCurveIndex[j]));
				t++;
				break;
			}
		}
		delete[] partialSums;
		delete[] partialSumsToCurveIndex;
	}
	for (int i = 0; i < k; i++)
		cout << "center " << i+1 << ": " << kCenters->iterate(i)->getCurveId() << endl;

	return kCenters;
}
