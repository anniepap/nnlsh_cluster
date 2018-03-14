#include <iostream>
#include <string>
#include <string.h>
#include <math.h>
#include <sstream>
#include <cstdlib>
#include "limits.h"

using namespace std;

double parseDouble(string s) {
    istringstream is(s);
    double f;
    is >> f;
    return f;
}

Limits GetLimits(Limits limits, Curve* curve) {
    for (int i = 0; i < curve->getNumOfPoints(); i++) {
        for (int j = 0; j < curve->getCurvePoint(0).dim; j++) {
            double coordinate = curve->getCurvePoint(i).p[j];
            if (coordinate < limits.minPoint.p[j])
                limits.minPoint.p[j] = coordinate;
            if (coordinate > limits.maxPoint.p[j])
                limits.maxPoint.p[j] = coordinate;
        }
    }
    return limits;
}

Limits getGridLimits(Limits limits) {
    Limits glimits = limits;
    int dim = limits.minPoint.dim;
    for (int i = 0; i < dim; i++) {
        glimits.minPoint.p[i] = floor(limits.minPoint.p[i]);
        glimits.maxPoint.p[i] = ceil(limits.maxPoint.p[i]);
    }
    return glimits;
}
