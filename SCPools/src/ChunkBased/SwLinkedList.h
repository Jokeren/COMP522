/*
 * SwLinkedList.h
 *
 *	Single writer linked list
 *	Uses HP 0 & 1
 *
 */

#ifndef SWLINKEDLIST_H_
#define SWLINKEDLIST_H_

#include "SPChunk.h"
#include "hp/hp.h"
#include "commons.h"


class SwNode {
public:
	SwNode(SPChunk* c);
	SPChunk* chunk;
	int consumerIdx;
	SwNode* next;
};

class SwLinkedList {
public:

	class SwLinkedListIterator {
	public:
		SwLinkedListIterator(SwLinkedList* list);
        OpResult next(SwNode *& node);
        void reset(SwLinkedList *list);
    private:
        SwNode *curr;
        int hp0, hp1;
        HP::HPLocal hpLoc;
    };
    friend class SwLinkedListIterator;
    SwLinkedList();

    virtual ~SwLinkedList();
    void append(SwNode *nodeToAdd);
    void remove(SwNode *nodeToRemove);
    void replace(SwNode *nodeToReplace, SwNode *newNode);
    SwNode *append(SPChunk *chunkToAdd);
    SwNode* getLast(HP::HPLocal hpLoc);

private:
    SwLinkedList(const SwLinkedList& other) { this->head = other.head; }
    SwLinkedList& operator=(const SwLinkedList& other) { this->head = other.head; return *this;}

private:
    SwNode *head;
    SwNode *last;
    SwNode *findPrevNode(SwNode *n);
};





#endif /* SWLINKEDLIST_H_ */
