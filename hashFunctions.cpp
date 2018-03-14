#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "hashFunctions.h"

using namespace std;

Hash::Hash(int numOfrs) {
	this->numOfrs = numOfrs;
}

Hash::~Hash() {
	delete[] r;
}

int Hash::HashFunction(double* concatVector) {}

LSHFunction::LSHFunction(int numOfrs) : Hash(numOfrs) {
	r = new int[K_VEC];
	for (int i = 0; i < K_VEC; ++i) {
		r[i] = rand()%((2*K_VEC)-1);
	}

	vectors = new int*[K_VEC];
	ts = new double[K_VEC];
	for (int i = 0; i < K_VEC; i++) {
		int* v = new int[numOfrs];
		for (int j = 0; j < numOfrs; j++) {
			v[j] = marsaglia();
		}
		double t = W*(rand()/(RAND_MAX + 1.0));		// uniformly between [0, w)
		vectors[i] = v;
		ts[i] = t;
	}
}

LSHFunction::~LSHFunction() {
	for (int i = 0; i < K_VEC; i++)
		delete[] vectors[i];
	delete[] vectors;
	delete[] ts;
}

int LSHFunction::HashFunction(double* concatVector) {
	int sum = 0;
	for (int i = 0; i < K_VEC; i++) {
		double h;
		double innerProduct = 0;
		for (int j = 0; j < numOfrs; j++)
			innerProduct += concatVector[j] * vectors[i][j];	//p·v

		h = (innerProduct + ts[i]) / W;
		sum += r[i] * floor(h);
	}
	return abs(sum) % M;
}

double LSHFunction::marsaglia() {

	double s, u, v;
	int m = -1;
	int n = 1;

	// u,v normal sto (-1,1)
	u = m + (rand()/(RAND_MAX + 1.0))*(n-m+1);
	v = m + (rand()/(RAND_MAX + 1.0))*(n-m+1);

	s = pow(u,2) + pow(v,2);
	while (s >= 1) {
		u = m + (rand()/(RAND_MAX + 1.0))*(n-m+1);
		v = m + (rand()/(RAND_MAX + 1.0))*(n-m+1);
		s = pow(u,2) + pow(v,2);
	}

	double x = u*sqrt(-2*log(s)/s);
 	while (x < 0) {
 		u = m + (rand()/(RAND_MAX + 1.0))*(n-m+1);
 		x = u*sqrt(-2*log(s)/s);
 	}
	return x;
}
