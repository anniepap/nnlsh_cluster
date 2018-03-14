#ifndef LIMITS_H
#define LIMITS_H
#include <string>
#include "curve.h"

using namespace std;

struct Limits {
	CurvePoint minPoint;
	CurvePoint maxPoint;
};

double parseDouble(string s);
Limits GetLimits(Limits limits, Curve* curve);
Limits getGridLimits(Limits limits);

#endif
