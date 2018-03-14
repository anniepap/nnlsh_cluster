#include <iostream>
#include "gridCurve.h"

using namespace std;

GridCurveNode::GridCurveNode(GridPoint& gp) {
	info = &gp;
	next = NULL;
}

GridCurve::GridCurve() {
	numberOfPoints = 0;
	head = NULL;
	tail = NULL;
}

GridCurve::~GridCurve() {
	GridCurveNode *t = head;
    while (t != NULL) {
        GridCurveNode *tp = t;
        t = t->next;
	   	tp->info = NULL;
        delete tp;
    }
    head = NULL;
    tail = NULL;
}

bool GridCurve::InsertNewPoint(GridPoint &gp) {
	if (!CheckConsecutiveDuplicates(gp)) {
		GridCurveNode *tp = new GridCurveNode(gp);
		if (head == NULL)
			head = tp;
		else
			tail->next = tp;
		tail = tp;
		numberOfPoints++;
		return true;
	}
	return false;
}

bool GridCurve::CheckConsecutiveDuplicates(GridPoint gp) {
	if (tail != NULL) {
		for (int i = 0; i < gp.point.dim; i++)
			if (tail->info->point.p[i] != gp.point.p[i])
				return false;
		return true;
	}
	else
		return false;
}

void GridCurve::GridCurvePrint() {
	GridCurveNode *t = head;
	cout.precision(17);
    while (t != NULL) {
    	t->info->point.print();
        t = t->next;
    }
}
