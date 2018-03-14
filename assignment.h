#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H
#include "limits.h"
#include "curve.h"
#include "curveList.h"
#include "clusterInfo.h"
#include "hashTable.h"

using namespace std;

bool CheckIfCenter(Curve* curve, int number_of_clusters, ClusterInfo** clusters);
ClusterInfo** LloydsAssignment(List* curveList, int number_of_clusters, string d, ClusterInfo** clusters, int flag);
GridHash* CreateLSHTables(int dim, int n, int min, int max, int number_of_grid_tables, int number_of_hash_tables, int number_of_clusters, Limits limits, List* curveList);
double initR(List* centers, string d, int number_of_clusters);
int searchInRange(ClusterInfo** clusters, GridHash* lshTables, Curve* center, int centerId, string d, double R, int number_of_hash_tables, int number_of_grid_tables, int number_of_clusters, int dim, int max);
ClusterInfo** ANN(GridHash* gridHash, ClusterInfo** clusters, List* curveList, List* centers, string d, int number_of_clusters, int number_of_hash_tables, int number_of_grid_tables, int dim, int max, int n);

#endif
