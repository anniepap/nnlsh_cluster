#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <vector>
#include "update.h"
#include "curveList.h"
#include "clusterInfo.h"
#include "distanceFunctions.h"

using namespace std;

void postOrderPrint(TreeNode* node) {
	if (node->left!= NULL) {
		cout << "LEFT" << endl;
		postOrderPrint(node->left);
	}
	if (node->right!=NULL) {
		cout << "RIGHT" << endl;
		postOrderPrint(node->right);
	}
	cout << "ROOT" << endl;
	node->curve->CurvePrint();
}


Curve* createMeanCurveTree(List* curves, Curve* center, string function) {

	cout << endl << "Mean Frechet" << endl;
	//if cluster is empty
	if (curves->getHead() == NULL)
		return center;

	vector<TreeNode*> currNodes;
	// Start with original curves as leaves
	Node* t = curves->getHead();
	while (t != NULL) {
	//for (int i = 0 ; i < curves->numOfNodes ; i++)
		currNodes.push_back(new TreeNode(t->info));
		t = t->next;
	}
	vector<TreeNode*> nextNodes;
	while (currNodes.size() > 1) {

		// Take curves in pairs
		for (int i = 0 ; i < currNodes.size() - 1; i+=2) {
			Curve* P = currNodes[i]->curve;
			Curve* Q = currNodes[i+1]->curve;
			Curve* meanCurve = meanDiscreteFrechetCurve(P, Q);
			TreeNode* meanNode = new TreeNode(meanCurve, currNodes[i], currNodes[i+1]);
			nextNodes.push_back(meanNode);
		}
		// if size is odd, there is an extra node at the end
		if (currNodes.size() % 2 == 1)
			nextNodes.push_back(currNodes[currNodes.size()-1]);

		currNodes = nextNodes;
		nextNodes.clear();
	}
	//postOrderPrint(currNodes[0]);
	currNodes[0]->curve->setDist(1000);
	currNodes[0]->curve->setCenterId(-1);
	cout << "new center" << endl;
	currNodes[0]->curve->CurvePrint();

	// return total root node
	return currNodes[0]->curve;
}

Curve* meanDiscreteFrechetCurve(Curve* P, Curve* Q) {
	vector<PointsPair> traversal = optimalTraversalComputation(P, Q);
	int numOfPoints = traversal.size();
	CurvePoint meanPoints[numOfPoints];

	// start from end (see comment in optimalTraversalComputation)
	int dim = traversal[0].p.dim;
	for (int i = numOfPoints-1 ; i >= 0 ; i--) {
		PointsPair pair = traversal[i];
		meanPoints[numOfPoints-1-i].dim = dim;
		for (int j = 0 ; j < dim ; j++)
			meanPoints[numOfPoints-1-i].p[j] = (pair.p.p[j] + pair.q.p[j]) / 2;
		//meanPoints[numOfPoints-1-i].print();
	}

	Curve* meanCurve = new Curve("MEANCURVE", numOfPoints, meanPoints);	// all mean Discrete Curves with same id = MEANCURVE
	return meanCurve;
}

int min3Index(double a, double b, double c) {
	int idx;
	double currMin;
	if (a <= b) {
		idx = 0;
		currMin = a;
	} else {
		idx = 1;
		currMin = b;
	}
	if (c <= currMin) {
		idx = 2;
		currMin = c;
	}
	return idx;
}

vector<PointsPair> optimalTraversalComputation(Curve* P, Curve* Q) {
	
	double** L = CreateDFDtable(P, Q);

	vector<PointsPair> traversal;
	int i = P->getNumOfPoints() - 1;
	int j = Q->getNumOfPoints() - 1;
	// put at the end of the vector, so we will read it in reverse afterwards
	traversal.push_back(PointsPair(P->getCurvePoint(i), Q->getCurvePoint(j)));

	while (i != 0 && j != 0) {
		int minIdx = min3Index(L[i-1][j], L[i][j-1], L[i-1][j-1]);
		if (minIdx == 0)
			i--;
		else if (minIdx == 1)
			j--;
		else if (minIdx == 2) {
			i--;
			j--;
		}
		traversal.push_back(PointsPair(P->getCurvePoint(i), Q->getCurvePoint(j)));
	}

	// Corner cases
	if (i != 0 && j == 0)
		while (i != 0) {
			i--;
			traversal.push_back(PointsPair(P->getCurvePoint(i), Q->getCurvePoint(j)));
		}
	if (i == 0 && j != 0)
		while (j != 0) {
			j--;
			traversal.push_back(PointsPair(P->getCurvePoint(i), Q->getCurvePoint(j)));
		}

	for (int n = 0; n < P->getNumOfPoints(); n++)
		delete[] L[n];
	delete[] L;

	return traversal;
}

void printCluster(vector<Curve*> curves, Curve* center) {
	cout << "center is " << center->getCurveId() << endl;
	// cout << "curves: ";
	// for (int i = 0; i < curves.size(); i++)
	// 	cout << curves[i]->getCurveId() << " ";
	// cout << endl;
}

double objectiveCostFunction(vector<Curve*> curves, Curve* center, string function, int flag) {
	double dist;
	double J = 0;		// Objective function
	for (int i = 0; i < curves.size(); i++) {
		if (function.compare("DFT") == 0)
			dist = FrechetDistance(curves[i], center);
		else if (function.compare("DTW") == 0)
			dist = WarpingDistance(curves[i], center);
		else
			dist = cRMSD(curves[i], center, flag);

		J += dist;
	}
	return J;
}

void improvedUpdatePAM(ClusterInfo* cluster, string function, int flag) {
	
	cout << endl << "PAM" << endl;
	List* curveList = cluster->getCurves();

	//if cluster is empty
	if (curveList->numOfNodes == 0)
		return;

	vector<Curve*> curves;
	Node* temp = curveList->getHead();
	while (temp != NULL) {
		curves.push_back(temp->info);
		temp = temp->next;
	}

	double minObjValue = objectiveCostFunction(curves, cluster->getCentroid(), function, flag);
	double objValue;
	int clusterId = curves[0]->getCenterId();

	//swap each curve in the cluster with the centroid
	for (int i = 0; i < curves.size(); i++) {
		Curve* oldCenter = cluster->getCentroid();
		Curve* newCenter = curves[i];
		curves[i] = oldCenter;
		cluster->setCentroid(newCenter);

		//keep as new centroid the curve the minimizes to dist to others
		objValue = objectiveCostFunction(curves, newCenter, function, flag);
		if (objValue < minObjValue) {
			minObjValue = objValue;
		}
		else {
			curves[i] = newCenter;
			cluster->setCentroid(oldCenter);
		}
	}

	//update cluster's list of curves
	curveList->deleteList();
    Curve* center = cluster->getCentroid();
    center->setCenterId(-1);
    center->setDist(10000);
	for (int i = 0; i < curves.size(); i++) {
		curves[i]->setCenterId(clusterId);
		curveList->SortingInsert(*curves[i]);
	}
	
	cout << "new ";
	cluster->printCluster();
}
