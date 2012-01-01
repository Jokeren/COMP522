/*
 * LinkedList.h
 *
 *  Created on: Dec 29, 2011
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include "Chunk.h"
#include "atomicMarkedReference.h"
#include "hp/hp.h"

class Node {
public:
	Node(Chunk* c);
	Chunk* chunk;
	int consIdx;
	markable_ref next;
};

class LFLinkedList {
public:
	LFLinkedList();
	virtual ~LFLinkedList();

	Node* append(Chunk* c, HPLocal hpLoc);
	Node* get(int idx, HPLocal hpLoc);
	bool remove(Node* toRemove, HPLocal hpLoc);
private:
	Node* head;
};

#endif /* LINKEDLIST_H_ */
