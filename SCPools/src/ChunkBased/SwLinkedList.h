/*
 * SwLinkedList.h
 *
 *	Single writer linked list
 *
 */

#ifndef SWLINKEDLIST_H_
#define SWLINKEDLIST_H_

#include "Chunk.h"

class SwNode {
public:
	SwNode(Chunk* c);
	Chunk* chunk;
	int consumerIdx;
	SwNode* next;
};

class SwLinkedList {
public:

	class SwLinkedListIterator {
	public:
		SwLinkedListIterator(SwLinkedList* list);
		//skips removed nodes but doesn't release them
		SwNode* next();
		//reset to head
		void reset(SwLinkedList* list);
	private:
		SwNode* curr;
	};

	friend class SwLinkedListIterator;

	SwLinkedList();
	virtual ~SwLinkedList();
	void append(SwNode* nodeToAdd);
	SwNode* append(Chunk* chunkToAdd);
	SwNode* getNode(int NodeIdx);


private:
	SwNode* head;
};





#endif /* SWLINKEDLIST_H_ */
