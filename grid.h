#ifndef GRID_H
#define GRID_H
#include "limits.h"
#include "curve.h"

struct GridPoint {
	CurvePoint point;
};

class Grid {
private:
	int dim;
	int dimSizes[4];
	GridPoint**** gridTable;
public:
	Grid(Limits, double);
	~Grid();
	void GridPrint();
	CurvePoint marsaglia(double, int);
	void ShiftGrid(CurvePoint);
	GridPoint* FindNearestGridPoint(CurvePoint, double);
};

#endif
