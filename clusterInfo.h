#ifndef CLUSTERINFO_H
#define CLUSTERINFO_H
#include <vector>
#include "curve.h"
#include "curveList.h"

class ClusterInfo {
private:
	Curve* centroid;
	List* curves;
public:
	ClusterInfo();
	~ClusterInfo();
	Curve* getCentroid();
	List* getCurves();
	void setCentroid(Curve*);
	int getNumOfCurves();

	void setCurves(List* curves);
	void setCurve(Curve* curve, int i);
	
	void InsertCurve(Curve*, double, int);
	void printCluster();
};

bool centroidsAreEqual(vector<Curve*> centroidsA, vector<Curve*> centroidsB);

#endif
