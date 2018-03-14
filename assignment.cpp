#include <iostream>
#include <string.h>
#include "assignment.h"
#include "gridCurve.h"
#include "distanceFunctions.h"

#define r 0.0005

bool CheckIfCenter(Curve* curve, int number_of_clusters, ClusterInfo** clusters) {

	bool check = false;
	for (int i = 0; i < number_of_clusters; i++) {
		if (clusters[i]->getCentroid()->getCurveId() == curve->getCurveId()) {
			check = true;
			break;
		}
	}
	return check;
}

ClusterInfo** LloydsAssignment(List* curveList, int number_of_clusters, string d, ClusterInfo** clusters, int flag) {

	cout << endl << "Lloyd's Assignment" << endl;
	int bestCentroid;
	double dist, newDist;
	Node* t = curveList->getHead();

	while (t != NULL) {
		//for all the curves of the dataset except the centroids
		if (!CheckIfCenter(t->info, number_of_clusters, clusters)) {
			dist = 10000;
			//find the center with the min dist to the curve
			for (int i = 0; i < number_of_clusters; i++) {
				if (d.compare("DFT") == 0)
					newDist = FrechetDistance(t->info, clusters[i]->getCentroid());
				else if (d.compare("DTW") == 0)
					newDist = WarpingDistance(t->info, clusters[i]->getCentroid());
				else
					newDist = cRMSD(t->info, clusters[i]->getCentroid(), flag);

				if (newDist <= dist) {
					dist = newDist;
					bestCentroid = i;
				}
			}
			if (t->info->getCenterId() == -1)					//first assignment to cluster
				clusters[bestCentroid]->InsertCurve(t->info, dist, bestCentroid);
			else if (t->info->getCenterId() == bestCentroid)	//already in this cluster, update dist
				t->info->setDist(dist);
			else {												//remove from previous cluster, assign to new
				clusters[t->info->getCenterId()]->getCurves()->remove(t->info);
				clusters[bestCentroid]->InsertCurve(t->info, dist, bestCentroid);
			}
		}
		t = t->next;
	}
	// for (int i = 0; i < number_of_clusters; i++)
	//  	clusters[i]->printCluster();

	return clusters;
}

GridHash* CreateLSHTables(int dim, int n, int min, int max, int number_of_grid_tables, int number_of_hash_tables, int number_of_clusters, Limits limits, List* curveList) {

	//calculate δ, vectorSize, hashTableSize
	double delta = 4*dim*min*r;
	int vectorSize = dim*number_of_grid_tables*max;
	int hashTableSize = n/8;
	cout << "delta is " << delta << endl;
	cout << "vectorSize is " << vectorSize << endl;
	cout << "hashTableSize is " << hashTableSize << endl;

	Hash* hashFunction = new LSHFunction(vectorSize);
	Limits glimits = getGridLimits(limits);

	//create an array of L structs to keep all grids and their hash tables
	GridHash* gridHash = new GridHash[number_of_hash_tables];
	gridHash->vectorSize = vectorSize;
	gridHash->hashTableSize = hashTableSize;
	gridHash->delta = delta;

	for (int l = 0; l < number_of_hash_tables; l++) {
		//create k grids
		gridHash[l].grids = new Grid*[number_of_grid_tables];
		for (int i = 0; i < number_of_grid_tables; i++) {
			gridHash[l].grids[i] = new Grid(glimits, delta);
			CurvePoint t = gridHash[l].grids[i]->marsaglia(delta, dim);
			gridHash[l].grids[i]->ShiftGrid(t);
		}
		//create HashTable
		gridHash[l].hashTable = new HashTable(hashTableSize, hashFunction);
	}

	//store curves in hash tables
	Node* t = curveList->getHead();
	while (t != NULL) {
		Curve* newCurve = t->info;
	    for (int l = 0; l < number_of_hash_tables; l++) {
	    	//concat array for k grid curves
			GridCurve* concat[number_of_grid_tables];
			//transform concat to vector
			double* concatVector = new double[vectorSize];
	    	for (int i = 0; i < vectorSize; i++)
				concatVector[i] = 0;

		    //find k grid curves for this curve and concat them
		    int z = 0;
		    GridPoint* nearestGridPoint;
		    for (int i = 0; i < number_of_grid_tables; i++) {
		    	int insertedCounter = 0;
		    	concat[i] = new GridCurve();
		    	for (int j = 0; j < newCurve->getNumOfPoints(); j++) {
		    		nearestGridPoint = gridHash[l].grids[i]->FindNearestGridPoint(newCurve->getCurvePoint(j), delta);
			    	if (concat[i]->InsertNewPoint(*nearestGridPoint)) {
			    		for (int i = 0; i < dim; i++, z++) {
			    			concatVector[z] = nearestGridPoint->point.p[i];
						}
			    		insertedCounter++;
			    	}
			    }
			    z += dim*(max - insertedCounter);
			    delete concat[i];
		    }
			//insert vector in hashTable
			gridHash[l].hashTable->insert(concatVector, vectorSize, newCurve);
		}
		t = t->next;
	}
	return gridHash;
}

double initR(List* centers, string d, int number_of_clusters) {

	double newDist;
	double R = 10000;

	//find the mid dist between the centroids
	for (int i = 0; i < number_of_clusters; i++) {
		for (int j = i+1; j < number_of_clusters; j++) {
			if (d.compare("DFT") == 0)
				newDist = FrechetDistance(centers->iterate(i), centers->iterate(j));
			else if (d.compare("DTW") == 0)
				newDist = WarpingDistance(centers->iterate(i), centers->iterate(j));

			if (newDist <= R)
				R = newDist;
		}
	}
	return R/2;
}

int searchInRange(ClusterInfo** clusters, GridHash* lshTables, Curve* center, int centerId, string d, double R, int number_of_hash_tables, int number_of_grid_tables, int number_of_clusters, int dim, int max) {

	int count = 0;
	for (int l = 0; l < number_of_hash_tables; l++) {
    	//concat array for number_of_grid_tables grid curves
		GridCurve* concat[number_of_grid_tables];
		//transform concat to vector
		double* concatVector = new double[lshTables->vectorSize];
    	for (int i = 0; i < lshTables->vectorSize; i++)
			concatVector[i] = 0;

	    //find number_of_grid_tables grid curves for this curve, concat them and transform concat to vector
	    int z = 0;
	    GridPoint* nearestGridPoint;
	    for (int k = 0; k < number_of_grid_tables; k++) {
	    	int insertedCounter = 0;
	    	concat[k] = new GridCurve();
	    	for (int j = 0; j < center->getNumOfPoints(); j++) {
	    		nearestGridPoint = (lshTables[l].grids[k]->FindNearestGridPoint(center->getCurvePoint(j), lshTables->delta));
		    	if (concat[k]->InsertNewPoint(*nearestGridPoint)) {
		    		for (int i = 0; i < dim; i++, z++)
		    			concatVector[z] = nearestGridPoint->point.p[i];
		    		insertedCounter++;
		    	}
		    }
		    z += dim*(max - insertedCounter);
	    }
	    //search curves in centroid's bucket that are in range R
		count += lshTables[l].hashTable->searchInRange(clusters, concatVector, lshTables->vectorSize, center, centerId, d, R, number_of_clusters);

		delete[] concatVector;
		for (int i = 0; i < number_of_grid_tables; i++)
	    	delete concat[i];
	}
	return count;
}

ClusterInfo** ANN(GridHash* gridHash, ClusterInfo** clusters, List* curveList, List* centers, string d, int number_of_clusters, int number_of_hash_tables, int number_of_grid_tables, int dim, int max, int n) {

	cout << endl << "ANN" << endl;
	int count;
	int bestCentroid;
	double dist, newDist;
	double R = initR(centers, d, number_of_clusters);

	//multiply R by two and search until no curves are found 
	do {
		count = 0;
		for (int i = 0; i < number_of_clusters; i++)
			count += searchInRange(clusters, gridHash, clusters[i]->getCentroid(), i, d, R, number_of_hash_tables, number_of_grid_tables, number_of_clusters, dim, max);
		R = R*2;
	} while (count != 0);

	Node* t = curveList->getHead();
	while (t != NULL) {
		//NN for curves that have not been assigned to clusters
		if (!CheckIfCenter(t->info, number_of_clusters, clusters) && !t->info->getInRange()) {
			dist = 10000;
			for (int i = 0; i < number_of_clusters; i++) {
				if (d.compare("DFT") == 0)
					newDist = FrechetDistance(t->info, clusters[i]->getCentroid());
				else if (d.compare("DTW") == 0)
					newDist = WarpingDistance(t->info, clusters[i]->getCentroid());

				if (newDist <= dist) {
					dist = newDist;
					bestCentroid = i;
				}
			}
			if (t->info->getCenterId() == -1)					//first assignment to cluster
				clusters[bestCentroid]->InsertCurve(t->info, dist, bestCentroid);
			else if (t->info->getCenterId() == bestCentroid)	//already in this cluster, update dist
				t->info->setDist(dist);
			else {												//remove from previous cluster, assign to new
				clusters[t->info->getCenterId()]->getCurves()->remove(t->info);
				clusters[bestCentroid]->InsertCurve(t->info, dist, bestCentroid);
			}
		}
		t->info->setInRange(false);
		t = t->next;
	}
	for (int i = 0; i < number_of_clusters; i++)
		clusters[i]->printCluster();

	return clusters;
}
