#include <iostream>
#include <string>
#include <math.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/SVD>
#include "distanceFunctions.h"
#include "curve.h"

using namespace std;
using namespace Eigen;

double EuclideanDistance(CurvePoint pointA, CurvePoint pointB) {

	double dist = 0.0;
	for (int i = 0; i < pointA.dim; i++) {
		double xi = pointB.p[i] - pointA.p[i];
		dist += xi*xi; //calculating Euclidean distance
	}
	return sqrt(dist);
}

double** CreateDFDtable(Curve *curveA, Curve *curveB) {

	double **L;
	int n = curveA->getNumOfPoints();
	int m = curveB->getNumOfPoints();
	int i, j;
	
	L = new double*[n];
	for (i = 0; i < n; i++)
		L[i] = new double[m];

	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++) {
			if (i == 0 && j == 0)
				L[i][j] = EuclideanDistance(curveA->getCurvePoint(0), curveB->getCurvePoint(0));
			else if (i == 0)
				L[i][j] = Max(EuclideanDistance(curveA->getCurvePoint(0), curveB->getCurvePoint(j)), L[0][j-1]);
			else if (j == 0)
				L[i][j] = Max(EuclideanDistance(curveA->getCurvePoint(i), curveB->getCurvePoint(0)), L[i-1][0]);
			else
				L[i][j] = Max(EuclideanDistance(curveA->getCurvePoint(i), curveB->getCurvePoint(j)), Min(Min(L[i-1][j], L[i][j-1]), L[i-1][j-1]));
		}
	}

	return L;
}

double FrechetDistance(Curve *curveA, Curve *curveB) {

	double **L = CreateDFDtable(curveA, curveB);
	int n = curveA->getNumOfPoints();
	int m = curveB->getNumOfPoints();
	double dist = L[n-1][m-1];
	for (int i = 0; i < n; i++)
		delete[] L[i];
	delete[] L;

	return dist;
}

double WarpingDistance(Curve *curveA, Curve *curveB) {

	double **L;
	int n = curveA->getNumOfPoints();
	int m = curveB->getNumOfPoints();
	int i, j;
	
	L = new double*[n];
	for (i = 0; i < n; i++)
		L[i] = new double[m];

	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++) {
			if (i == 0 && j == 0)
				L[i][j] = EuclideanDistance(curveA->getCurvePoint(0), curveB->getCurvePoint(0));
			else if (i == 0)
				L[i][j] = EuclideanDistance(curveA->getCurvePoint(0), curveB->getCurvePoint(j)) + L[0][j-1];
			else if (j == 0)
				L[i][j] = EuclideanDistance(curveA->getCurvePoint(i), curveB->getCurvePoint(0)) + L[i-1][0];
			else
				L[i][j] = EuclideanDistance(curveA->getCurvePoint(i), curveB->getCurvePoint(j)) + Min(Min(L[i-1][j], L[i][j-1]), L[i-1][j-1]);
		}
	}

	double dist = L[n-1][m-1];
	for (i = 0; i < n; i++)
		delete[] L[i];
	delete[] L;

	return dist;
}

double Max(double a, double b) {
	if (a > b)
		return a;
	return b;
}

double Min(double a, double b) {
	if (a < b)
		return a;
	return b;
}

double cRMSD(Curve* curveX, Curve* curveY, int function) {

	int i, j;
	int N = Min(curveX->getNumOfPoints(), curveY->getNumOfPoints());
	int dim = (curveX->getPoints())[0].dim;

	if (dim == 2) {
		if (function == 1)
			return FrechetDistance(curveX, curveY);
		else if (function == 2)
			return WarpingDistance(curveX, curveY);
	}

	MatrixXd X = Map<Matrix<double, Dynamic, Dynamic, RowMajor>> (curveX->getMovedPoints(), N, dim);
	MatrixXd Y = Map<Matrix<double, Dynamic, Dynamic, RowMajor>> (curveY->getMovedPoints(), N, dim);
	JacobiSVD<MatrixXd> svd(X.transpose()*Y, ComputeFullU | ComputeFullV);
	JacobiSVD<MatrixXd>::SingularValuesType singular = svd.singularValues();
	MatrixXd U = svd.matrixU();
	MatrixXd V = svd.matrixV();

	// cout << endl << "X" << endl << X << endl;
	// cout << endl << "Y" << endl << Y << endl << endl;
	// cout << endl << "Xtrans" << endl << X.transpose() << endl;
	// cout << endl << "Xtrans*Y" << endl << X.transpose()*Y << endl;
	// cout << endl << "U" << endl << U << endl;
	// cout << endl << "V" << endl << V << endl;
	// cout << endl << "singularValues" << endl << svd.singularValues() << endl;

	if (singular(dim-1) <= 0) {
		cout << "error: last singularValue <= 0" << endl;
		return -1;
	}

	MatrixXd Q = V.transpose()*U;
	if (Q.determinant() < 0) {
		U.col(dim-1) *= -1;
		// cout << endl << "U" << endl << U << endl;
		Q = V.transpose()*U;
	}
	MatrixXd XQ = X*Q;

	// cout << endl << "Q" << endl << Q << endl;
	// cout << endl << "XQ" << endl << X*Q << endl;

	if (function == 0) {
		return ((XQ-Y).norm())/sqrt(N);
	}
	else {
		double* points = new double[dim*N];
		Map<MatrixXd>(points, XQ.rows(), XQ.cols()) = XQ;
		string XQpoints = "";
		for (j = 0; j < dim*N; j++) {
			stringstream num;
			num << points[j];
			// cout << "num " << num.str() << endl;
			XQpoints += num.str() + "\t";
		}
		// cout << "XQpoints" << endl << XQpoints << endl;
		delete[] points;

		points = new double[dim*N];
		Map<MatrixXd>(points, Y.rows(), Y.cols()) = Y;
		// cout << endl << "points" << endl;
		string Ypoints = "";
		for (j = 0; j < dim*N; j++) {
			stringstream num;
			num << points[j];
			Ypoints += num.str() + "\t";
		}
		// cout << "Ypoints" << endl << Ypoints << endl;
		delete[] points;

		Curve* XQcurve = new Curve("XQcurve", N, XQpoints, dim, 10000, -1, false);
		Curve* Ycurve = new Curve("Ycurve", N, Ypoints, dim, 10000, -1, false);

		double dist;
		if (function == 1)
			dist = FrechetDistance(XQcurve, Ycurve);
		else if (function == 2)
			dist = WarpingDistance(XQcurve, Ycurve);
		// cout << endl;
		// XQcurve->CurvePrint();
		// Ycurve->CurvePrint();
		
		delete XQcurve;
		delete Ycurve;
		return dist;
	}
}
