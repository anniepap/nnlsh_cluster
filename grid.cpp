#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "curve.h"
#include "grid.h"
#include "limits.h"
#include "distanceFunctions.h"

using namespace std;

Grid::Grid(Limits limits, double delta) {
	dim = limits.minPoint.dim;
	for (int i = 0; i < dim; i++)
		dimSizes[i] = (int)((limits.maxPoint.p[i] - limits.minPoint.p[i])/delta) + 1;
	for (int i = dim; i < 4; i++)
		dimSizes[i] = 1;
	
	CurvePoint step = limits.minPoint;

	gridTable = new GridPoint***[dimSizes[0]];
	for (int i = 0; i < dimSizes[0]; i++) {
		gridTable[i] = new GridPoint**[dimSizes[1]];
		for (int j = 0; j < dimSizes[1]; j++) {
			gridTable[i][j] = new GridPoint*[dimSizes[2]];
			for (int k = 0; k < dimSizes[2]; k++) {
				gridTable[i][j][k] = new GridPoint[dimSizes[3]];
				for (int l = 0; l < dimSizes[3]; l++) {
					CurvePoint cp = step;
					gridTable[i][j][k][l].point = cp;
					step.p[3] += delta;
				}
				step.p[2] += delta;
				step.p[3] = limits.minPoint.p[3];
			}
			step.p[1] += delta;
			step.p[2] = limits.minPoint.p[2];
			step.p[3] = limits.minPoint.p[3];
		}
		step.p[0] += delta;
		step.p[1] = limits.minPoint.p[1];
		step.p[2] = limits.minPoint.p[2];
		step.p[3] = limits.minPoint.p[3];
	}
}

Grid::~Grid() {
	for (int i = 0; i < dimSizes[0]; i++) {
		for (int j = 0; j < dimSizes[1]; j++) {
			for (int k = 0; k < dimSizes[2]; k++) {
				delete[] gridTable[i][j][k];
			}
			delete[] gridTable[i][j];
		}
		delete[] gridTable[i];
	}
	delete[] gridTable;
}

void Grid::GridPrint() {
	for (int i = 0; i < dimSizes[0]; i++)
		for (int j = 0; j < dimSizes[1]; j++)
			for (int k = 0; k < dimSizes[2]; k++)
				for (int l = 0; l < dimSizes[3]; l++) {
					cout << "(";
					gridTable[i][j][k][l].point.print();
					cout << ")" << endl;
				}
}

CurvePoint Grid::marsaglia(double delta, int dim) {

	CurvePoint cp;
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

	for (int i = 0; i < dim; i++) {
		double xi = u*sqrt(-2*log(s)/s);
		while (xi < 0 || xi >= delta) {
			u = m + (rand()/(RAND_MAX + 1.0))*(n-m+1);
			xi = u*sqrt(-2*log(s)/s);
		}
		cp.p[i] = xi;
	}

	return cp;
}

void Grid::ShiftGrid(CurvePoint t) {
	for (int i = 0; i < dimSizes[0]; i++)
		for (int j = 0; j < dimSizes[1]; j++)
			for (int k = 0; k < dimSizes[2]; k++)
				for (int l = 0; l < dimSizes[3]; l++) {
					gridTable[i][j][k][l].point.p[0] += t.p[0];
					gridTable[i][j][k][l].point.p[1] += t.p[1];
					gridTable[i][j][k][l].point.p[2] += t.p[2];
					gridTable[i][j][k][l].point.p[3] += t.p[3];
				}
}

GridPoint* Grid::FindNearestGridPoint(CurvePoint cpoint, double delta) {
	GridPoint *gpoint;
	double dist = 10000.00;
	int xMin, xMax, yMin, yMax, zMin, zMax, wMin, wMax;

	xMin = (cpoint.p[0] - gridTable[0][0][0][0].point.p[0])/delta;
	xMax = (gridTable[dimSizes[0]-1][0][0][0].point.p[0] - cpoint.p[0])/delta;

	yMin = (cpoint.p[1] - gridTable[0][0][0][0].point.p[1])/delta;
	yMax = (gridTable[0][dimSizes[1]-1][0][0].point.p[1] - cpoint.p[1])/delta;

	zMin = 0;
	zMax = 0;
	wMin = 0;
	wMax = 0;

	if (dim == 3) {
		zMin = (cpoint.p[2] - gridTable[0][0][0][0].point.p[2])/delta;
		zMax = (gridTable[0][0][dimSizes[2]-1][0].point.p[2] - cpoint.p[2])/delta;
	}
	else if (dim == 4) {
		wMin = (cpoint.p[3] - gridTable[0][0][0][0].point.p[3])/delta;
		wMax = (gridTable[0][0][0][dimSizes[3]-1].point.p[3] - cpoint.p[3])/delta;
	}

	for (int i = xMin; i < dimSizes[0]-xMax; i++)
		for (int j = yMin; j < dimSizes[1]-yMax; j++)
			for (int k = zMin; k < dimSizes[2]-zMax; k++)
				for (int l = wMin; l < dimSizes[3]-wMax; l++) {
					double euDist = EuclideanDistance(cpoint, gridTable[i][j][k][l].point);
					if (euDist < dist) {
						gpoint = &gridTable[i][j][k][l];
						dist = euDist;
					}
				}

	return gpoint;
}
