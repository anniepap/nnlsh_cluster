#include <iostream>
#include <fstream>
#include <string>
#include "bucketList.h"
#include "distanceFunctions.h"
#include "curveList.h"
#include "assignment.h"

using namespace std;

BucketInfo::BucketInfo(double* concatVector, int vectorSize, Curve& realCurve) {
	this->concatVector = concatVector;
	this->vectorSize = vectorSize;
	this->realCurve = &realCurve;
}

BucketInfo::~BucketInfo() {
	delete[] concatVector;
}

void BucketInfo::print() {
	// cout << "Vector ";
	// for (int i = 0; i < vectorSize; i++) {
	// 	cout << concatVector[i] << ", ";
	// }
	// cout << " in bucket" << endl;
	cout << realCurve->getCurveId() << endl;
}

BucketNode::BucketNode(BucketInfo* bi) {
	info = bi;
	next = NULL;
}

Bucket::Bucket() {
	head = NULL;
	tail = NULL;
}

Bucket::~Bucket() {
	BucketNode *t = head;
    while (t != NULL) {
        BucketNode *tp = t;
        t = t->next;
        delete tp->info;
        delete tp;
    }
    head = NULL;
    tail = NULL;
}

void Bucket::insert(BucketInfo* bi) {
	BucketNode *tp = new BucketNode(bi);
	if (head == NULL)
		head = tp;
	else
		tail->next = tp;
	tail = tp;
}

void Bucket::print() {
	BucketNode *t = head;
	while (t != NULL) {
		t->info->print();
		t = t->next;
	}
}

Result Bucket::search(double* vector, int vsize, Curve* queryCurve, string function) {

	double newDist;
	double dist = 10000.00;
	bool sameV = false;
	Curve* nnCurve = NULL;
	BucketNode *t = head;
	
	while (t != NULL) {
		bool found = true;
		if (t->info->vectorSize == vsize) {
			for (int i = 0; i < t->info->vectorSize; i++) {
				if (t->info->concatVector[i] != vector[i])
					found = false;
			}
			if (found) {
				sameV = true;
				if (function.compare("DFT") == 0)
					newDist = FrechetDistance(t->info->realCurve, queryCurve);
				else if (function.compare("DTW") == 0)
					newDist = WarpingDistance(t->info->realCurve, queryCurve);
				if (newDist < dist) {
					dist = newDist;
					nnCurve = t->info->realCurve;
				}
			}
		}
		t = t->next;
	}

	//if not one curve with the same v is not found search all bucket nodes
	if (!sameV) {
		t = head;
		while (t != NULL) {
			if (function.compare("DFT") == 0)
				newDist = FrechetDistance(t->info->realCurve, queryCurve);
			else if (function.compare("DTW") == 0)
				newDist = WarpingDistance(t->info->realCurve, queryCurve);
			if (newDist < dist) {
				dist = newDist;
				nnCurve = t->info->realCurve;
			}
			t = t->next;
		}
	}

	Result r(nnCurve, dist, sameV);
	return r;
}

int Bucket::searchInRange(ClusterInfo** clusters, double* vector, int vsize, Curve* queryCurve, int centerId, string function, double R, int number_of_clusters) {

	int count = 0;
	double newDist;
	bool sameV = false;
	BucketNode *t = head;
	
	while (t != NULL) {
		if (!CheckIfCenter(t->info->realCurve, number_of_clusters, clusters)) {//) && !t->info->realCurve->getInRange()) {
			if (function.compare("DFT") == 0)
				newDist = FrechetDistance(t->info->realCurve, queryCurve);
			else if (function.compare("DTW") == 0)
				newDist = WarpingDistance(t->info->realCurve, queryCurve);

			if (newDist <= R && newDist < t->info->realCurve->getDist()) {
				t->info->realCurve->setInRange(true);
				if (t->info->realCurve->getCenterId() == -1)						//first assignment to cluster
					clusters[centerId]->InsertCurve(t->info->realCurve, newDist, centerId);
				else if (t->info->realCurve->getCenterId() == centerId)		//already in this cluster, update dist
					t->info->realCurve->setDist(newDist);
				else {												//remove from previous cluster, assign to new
					clusters[t->info->realCurve->getCenterId()]->getCurves()->remove(t->info->realCurve);
					clusters[centerId]->InsertCurve(t->info->realCurve, newDist, centerId);
				}
				count++;
			}
		}
		t = t->next;
	}
	return count;
}
