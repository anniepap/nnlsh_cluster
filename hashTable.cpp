#include <iostream>
#include "curve.h"
#include "hashTable.h"
#include "hashFunctions.h"
#include "bucketList.h"

using namespace std;

HashTable::HashTable(int htSize, Hash* hf) {
	hashTableSize = htSize;
	buckets = new Bucket[hashTableSize];
	this->hf = hf;
}

HashTable::~HashTable() {
	delete[] buckets;
}

void HashTable::insert(double* vector, int vsize, Curve* c) {
	int bucketId = hf->HashFunction(vector) % hashTableSize;	// (r1h1(p) + r2h2(p) + · · · + rkhk (p) mod M) mod TableSize
	//cout << "INSERT AT " << bucketId << endl;
	BucketInfo* info = new BucketInfo(vector, vsize, *c);
	buckets[bucketId].insert(info);
}

void HashTable::print() {
	for (int i = 0; i < hashTableSize; i++){
		cout << "Bucket " << i << endl;
		buckets[i].print();
	}
}

Result HashTable::search(double* vector, int vsize, Curve* c, string function) {
	int bucketId = hf->HashFunction(vector) % hashTableSize;
	cout << "SEARCH AT " << bucketId << endl;
	return buckets[bucketId].search(vector, vsize, c, function);
}

int HashTable::searchInRange(ClusterInfo** clusters, double* vector, int vsize, Curve* c, int centerId, string function, double R, int number_of_clusters) {
	int bucketId = hf->HashFunction(vector) % hashTableSize;
	//cout << "SEARCH IN RANGE AT " << bucketId << endl;
	return buckets[bucketId].searchInRange(clusters, vector, vsize, c, centerId, function, R, number_of_clusters);
}

Result HashTable::ExhaustiveSearch(Curve* c, string function) {
	Result result;
	for (int i = 0; i < hashTableSize; i++) {
		Result r = buckets[i].search(NULL, 0, c, function);
		if (r.dist < result.dist)
			result = r;
	}
	return result;
}
