#ifndef DISTANCEFUNCTIONS_H
#define DISTANCEFUNCTIONS_H
#include "curve.h"

double EuclideanDistance(CurvePoint pointA, CurvePoint pointB);
double FrechetDistance(Curve *curveA, Curve *curveB);
double WarpingDistance(Curve *curveA, Curve *curveB);
double Max(double a, double b);
double Min(double a, double b);
double** CreateDFDtable(Curve *curveA, Curve *curveB);
double cRMSD(Curve* curveX, Curve* curveY, int function);

#endif
