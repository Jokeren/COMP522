/*
 * LinkedList.h
 *
 *  Created on: Dec 29, 2011
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include "SPChunk.h"
#include "atomicMarkedReference.h"

class Node {
public:
	Node(SPChunk* c);
	SPChunk* chunk;
	int consumerIdx;
	markable_ref next;
};

// multi-writer linked list
class MwLinkedList {
public:
	MwLinkedList();
	virtual ~MwLinkedList();

	Node* append(SPChunk* c);
	Node* append(SPChunk* c, int consumerIdx);
	Node* get(int idx);
	bool remove(Node* toRemove);
private:
	Node* head;
};

#endif /* LINKEDLIST_H_ */
