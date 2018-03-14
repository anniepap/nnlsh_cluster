#ifndef GRIDCURVE_H
#define GRIDCURVE_H
#include "grid.h"

struct GridCurveNode {  	// nearest grid point to curve point
	GridPoint* info; 
	GridCurveNode* next;
	GridCurveNode(GridPoint&);
};

class GridCurve {			// curve on grid for input curve
private:
	GridCurveNode* head;
	GridCurveNode* tail;
public:
	int numberOfPoints;
	GridCurve();
	~GridCurve();
	bool InsertNewPoint(GridPoint&);
	bool CheckConsecutiveDuplicates(GridPoint);
	void GridCurvePrint();
};

#endif