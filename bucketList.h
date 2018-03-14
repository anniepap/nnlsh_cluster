#ifndef BUCKETLIST_H
#define BUCKETLIST_H
#include "curve.h"
#include "curveList.h"
#include "clusterInfo.h"

using namespace std;

class List;

struct Result {
	Curve* curve;
	double dist;
	bool foundGridCurve;
	Result() {
		curve = NULL;
		dist = 10000.00;
		foundGridCurve = false;
	}
	Result(Curve* curve, double dist, bool foundGridCurve) {
		this->curve = curve;
		this->dist = dist;
		this->foundGridCurve = foundGridCurve;
	}
};

struct BucketInfo {
	double* concatVector; 		// vector from concatanation of grid curves
	int vectorSize;
	Curve* realCurve;
	void print();
	BucketInfo(double* concatVector, int vectorSize, Curve& realCurve);
	~BucketInfo();
};

struct BucketNode {
	BucketInfo* info;
	BucketNode* next;
	BucketNode(BucketInfo*);
};

class Bucket {
private:
	BucketNode* head;
	BucketNode* tail;
public:
	Bucket();
	~Bucket();
	void insert(BucketInfo*);
	void print();
	Result search(double*, int, Curve*, string);
	int searchInRange(ClusterInfo**, double*, int, Curve*, int, string, double, int);
};

#endif
