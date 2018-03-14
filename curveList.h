#ifndef CURVELIST_H
#define CURVELIST_H
#include "curve.h"

struct Node {
	Curve* info;
	Node* next;
	Node(Curve&);
};

class List {
private:
	Node* head;
	Node* tail;
public:
	int numOfNodes;
	List();
	~List();
	Node* getHead();
	void ListInsert(Curve&);
	void SortingInsert(Curve&);
	bool CheckIfExists(Curve*);
	void InsertWithoutDups(List*);
	void ListPrint();
	string getCurveIds();
	Curve* getCurve(int);
	void setCurve(Curve* curve, int i);
	Curve* iterate(int);
	void remove(Curve*);
	void cleanInfo();
	void deleteList();
};

#endif
