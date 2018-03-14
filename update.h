#ifndef UPDATE_H
#define UPDATE_H
#include <vector>
#include "curve.h"
#include "curveList.h"
#include "clusterInfo.h"

using namespace std;

struct PointsPair {
	CurvePoint p;
	CurvePoint q;

	PointsPair(CurvePoint p, CurvePoint q) {
		this->p = p;
		this->q = q;
	}
};

struct TreeNode {
	Curve* curve;
	TreeNode* left;
	TreeNode* right;

	TreeNode(Curve* curve, TreeNode* left = NULL, TreeNode* right = NULL) {
		this->curve = curve;
		this->left = left;
		this->right = right;
	}

	bool isLeaf() {
		if (left == NULL && right == NULL)
			return true;
		else
			return false;
	}
};

void postOrderPrint(TreeNode* node);
Curve* createMeanCurveTree(List* curves, Curve* center, string function);
Curve* meanDiscreteFrechetCurve(Curve* P, Curve* Q);
vector<PointsPair> optimalTraversalComputation(Curve* P, Curve* Q);
double objectiveCostFunction(vector<Curve*> curves, Curve* center, string function, int flag);
void improvedUpdatePAM(ClusterInfo* cluster, string function, int flag);
int min3Index(double a, double b, double c);

#endif