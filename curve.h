#ifndef CURVE_H
#define CURVE_H
#include <string>

using namespace std;

struct CurvePoint {
	int dim;
	double p[4];

	void print();
	string printPoint();

	CurvePoint(double lat, double lon) {
		dim = 2;
		p[0] = lat;
		p[1] = lon;
	}

	CurvePoint(){}

	static CurvePoint initMin(int dim) {
		CurvePoint min;
		min.dim = dim;
		for (int i = 0; i < dim; i++)
			min.p[i] = 1000.00;
		return min;
	}
	static CurvePoint initMax(int dim) {
		CurvePoint max;
		max.dim = dim;
		for (int i = 0; i < dim; i++)
			max.p[i] = -1000.00;
		return max;
	}
};

class Curve {
private:
	string curveId;
	int m;			// numberOfPoints
	CurvePoint* points;
	double* movedPoints;
	//info about cluster center
	double dist;
	int centerId; // cluster ID, not center
	bool inRange;
public:
	Curve(string, int, CurvePoint*);
	Curve(string, int, string, int, double, int, bool);
	~Curve();
	void CurvePrint();
	string PrintPoints();
	
	CurvePoint getCurvePoint(int);
	int getNumOfPoints();
	string getCurveId();
	int getCenterId();
	double getDist();
	bool getInRange();
	CurvePoint* getPoints();
	double* getMovedPoints();

	void setCurveId(string);
	void setCenterId(int);
	void setDist(double);
	void setInRange(bool);

	bool comparePoints(Curve*);
};

#endif
