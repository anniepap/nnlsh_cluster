#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "curve.h"
#include "bucketList.h"
#include "hashFunctions.h"
#include "grid.h"
#include "clusterInfo.h"

class HashTable {
private:
	int hashTableSize;
	Bucket* buckets;
	Hash* hf;
public:
	HashTable(int, Hash*);
	~HashTable();
	void insert(double*, int, Curve*);
	void print();
	Result search(double*, int, Curve*, string);
	int searchInRange(ClusterInfo**, double*, int, Curve*, int, string, double, int);
	Result ExhaustiveSearch(Curve*, string);
};

struct GridHash {
	double delta;
	int vectorSize;
	int hashTableSize;
	Grid** grids;
	HashTable* hashTable;
};

#endif
