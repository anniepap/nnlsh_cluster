#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <cstdlib>
#include "curve.h"
#include "limits.h"

using namespace std;

void CurvePoint::print() {
    for (int i = 0; i < dim; i++)
        cout << p[i] << " ";
    cout << endl;
}

string CurvePoint::printPoint() {
    ostringstream os;
    os << "(";
    for (int i = 0; i < dim; i++) {
        os << p[i];
        if (i != dim-1)
            os << ", ";
    }
    os << ")";
    return os.str();
}

Curve::Curve(string cId, int numberOfPoints, string strOfPoints, int dim, double dist, int centerId, bool inRange) {
    curveId = cId;
    m = numberOfPoints;
    points = new CurvePoint[m];
    this->dist = dist;
    this->centerId = centerId;
    this->inRange = inRange;

    char *dup = strdup(strOfPoints.c_str());
    char *str = dup;
    for (int i = 0; i < m; i++) {
        points[i].dim = dim;
        for (int j = 0 ; j < dim; j++) {
            points[i].p[j] = parseDouble(strtok(str, "\t,"));
            str = NULL;
        }
    }
    free(dup);

    //calculate the mean centroid of the curve's points
    double* center = new double[dim];
    for (int i = 0; i < dim; i++)
        center[i] = 0;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < dim; j++)
            center[j] += points[i].p[j];
    }
    for (int i = 0; i < dim; i++)
        center[i] = center[i] / m;

    //subtrack the centroid from each point
    double** newCurve = new double*[m];
    for (int i = 0; i < m; i++)
        newCurve[i] = new double[dim];
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < dim; j++)
            newCurve[i][j] = points[i].p[j] - center[j];
    }

    //convert into 1d array for convenience
    int pos = 0;
    movedPoints = new double[m*dim];
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < dim; j++) {
            movedPoints[pos] = newCurve[i][j];
            //cout << movedPoints[pos] << endl;
            pos++;
        }
    }

    for (int i = 0; i < m; i++)
        delete newCurve[i];
    delete[] newCurve;
    delete[] center;
}

Curve::Curve(string cId, int numberOfPoints, CurvePoint* points) {
    curveId = cId;
    m = numberOfPoints;
    this->points = new CurvePoint[m];

    for (int i = 0; i < m; i++) {
        this->points[i] = points[i];
    }
}

Curve::~Curve() {
    delete[] points;
    delete[] movedPoints;
}

void Curve::CurvePrint() {
    cout << "curveId is: " << curveId << endl;
    cout << "numberOfPoints is: " << m << endl;
    cout << "curve's points are:" << endl;
    cout.precision(17);
    for (int i = 0; i < m; i++)
       points[i].print();
    // cout << "dist: " << dist << endl;
    // cout << "centerId: " << centerId << endl;
    cout << endl;
}

string Curve::PrintPoints() {
    string str = "";
    for (int i = 0; i < m; i++) {
        str += points[i].printPoint();
        if (i != m-1)
            str += ", ";
    }
    return str;
}

CurvePoint Curve::getCurvePoint(int m) {
    return points[m];
}

int Curve::getNumOfPoints() {
    return m;
}

string Curve::getCurveId() {
    return curveId;
}

CurvePoint* Curve::getPoints() {
    return points;
}

int Curve::getCenterId() {
    return centerId;
}

double Curve::getDist() {
    return dist;
}

bool Curve::getInRange() {
    return inRange;
}

double* Curve::getMovedPoints() {
    return movedPoints;
}

void Curve::setCurveId(string id) {
    curveId = id;
}

void Curve::setCenterId(int c) {
    centerId = c;
}

void Curve::setDist(double d) {
    dist = d;
}

void Curve::setInRange(bool r) {
    inRange = r;
}

bool Curve::comparePoints(Curve* curve) {
    if (m != curve->m)
        return false;
    else {
        for (int i = 0; i < m; i ++) {
            for (int d = 0; d < points[i].dim; d++) {
                if (points[i].p[d] != curve->points[i].p[d])
                    return false;
            }
        }
    }
    return true;
}
