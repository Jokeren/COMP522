/*
 * LinkedList.h
 *
 *  Created on: Dec 29, 2011
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

class Node {
public:
	Node(Chunk* c, bool isSentinal);

	Chunk* chunk;
	int consIdx;
	markable_ref next;
	bool isSentinal;
};

class LFLinkedList {
public:
	LFLinkedList();
	virtual ~LFLinkedList();

	Node* append(Chunk* c);
	Node* get(int idx);
	bool remove(Node* toRemove);
};

#endif /* LINKEDLIST_H_ */
