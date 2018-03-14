#ifndef INITKMEANS_H
#define INITKMEANS_H
#include "curve.h"
#include "curveList.h"

List* RandomSelection(int k, int N, List *curveList);
List* kMeansInit(int k, int totalCurves, List *curveList, string function);

#endif
