#include <iostream>
#include <string>
#include <cstdlib>
#include "curveList.h"
#include "curve.h"

using namespace std;

Node::Node(Curve& c) {
	info = &c;
	next = NULL;
}

List::List() {
	numOfNodes = 0;
	head = NULL;
	tail = NULL;
}

List::~List() {
	Node *t = head;
    while (t != NULL) {
        Node *tp = t;
        t = t->next;
        delete tp->info;
        delete tp;
    }
    head = NULL;
    tail = NULL;
}

Node* List::getHead() {
	return head;
}

void List::ListInsert(Curve &c) {
	Node *tp = new Node(c);
	if (head == NULL)
		head = tp;
	else
		tail->next = tp;
	tail = tp;
	numOfNodes++;
}

void List::SortingInsert(Curve &c) {
	Node *tp = new Node(c);
	Node *t = head;
	numOfNodes++;

	if (head != NULL && stoi(head->info->getCurveId()) > stoi(tp->info->getCurveId())) {
		tp->next = head;
		head = tp;
	}
 	else if (head == NULL) {
		head = tp;
		tail = tp;
	}
 	else {
		while (t->next != NULL && stoi(t->next->info->getCurveId()) < stoi(tp->info->getCurveId()))
			t = t->next;

		if (t->next != NULL) {
			tp->next = t->next;
			t->next = tp;
		}
		else {
			tail = tp;
			t->next = tp;
			tp->next = NULL;
		}
	}
}

bool List::CheckIfExists(Curve* check) {
	bool found = false;
	Node *t = head;
	while (t != NULL) {
		Curve* c = t->info;
		found = (c->getCurveId() == check->getCurveId());
		if (found)
			break;
		t = t->next;
	}
	return found;
}

void List::InsertWithoutDups(List* Results) {
	Node *t = Results->getHead();
	while (t != NULL) {
		Curve* c = t->info;
		if (!CheckIfExists(c))
			SortingInsert(*(t->info));
		t = t->next;
	}
}

void List::ListPrint() {
	Node *t = head;
	while (t != NULL) {
		Curve &c = *(t->info);
		c.CurvePrint();
		t = t->next;
	}
}

string List::getCurveIds() {
	string curveIds("");
	Node *t = head;
	while (t != NULL) {
		curveIds += t->info->getCurveId();
		if (t->next != NULL)
			curveIds += "\t";
		t = t->next;
	}
	return curveIds;
}

Curve* List::getCurve(int id) {
	Node *t = head;
	Curve *c;
	while (t != NULL) {
		c = t->info;
		if (atoi(c->getCurveId().c_str()) == id)
			break;
		t = t->next;
	}
	return c;
}

void List::setCurve(Curve* curve, int i) {
	Node *t = head;
	int count = 0;
	while (t != NULL) {
		if (count == i) {
			t->info = curve;
			break;
		}
		t = t->next;
		count++;
	}
}

Curve* List::iterate(int i) {
	Node *t = head;
	Curve *c = NULL;
	int count = 0;
	while (t != NULL && count <= i) {
		c = t->info;
		t = t->next;
		count++;
	}
	return c;
}

void List::remove(Curve* curve) {
	if (CheckIfExists(curve)) {
		Node *t = head;
		Node *prev = NULL;
		while (t != NULL) {
			if (curve->getCurveId() == t->info->getCurveId()) {
				if (prev == NULL)
					head = t->next;
				else
					prev->next = t->next;
				
				if (t->next == NULL)
					tail = prev;
				
				delete t;
				break;
			}
			prev = t;
			t = t->next;
		}
		numOfNodes--;
	}
}

void List::cleanInfo() {
	Node* t = head;
	while (t != NULL) {
		t->info->setCenterId(-1);
		t->info->setDist(10000);
		t->info->setInRange(false);
		t = t->next;
	}
}

void List::deleteList() {
	Node *t = head;
    while (t != NULL) {
        Node *tp = t;
        t = t->next;
        tp->info = NULL;
        delete tp;
    }
    head = NULL;
    tail = NULL;
    numOfNodes = 0;
}
