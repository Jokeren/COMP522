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
		SwLinkedListIterator(SwLinkedList* list, HP::HPLocal _hpLoc);
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
    void append(SwNode *nodeToAdd, HP::HPLocal hpLoc);
    void remove(SwNode *nodeToRemove, HP::HPLocal hpLoc);
    void replace(SwNode *nodeToReplace, SwNode *newNode, HP::HPLocal hpLoc);
    SwNode *append(SPChunk *chunkToAdd, HP::HPLocal hpLoc);
    SwNode* getLast(HP::HPLocal hpLoc);

private:
    SwLinkedList(const SwLinkedList& other) { this->head = other.head; }
    SwLinkedList& operator=(const SwLinkedList& other) { this->head = other.head; return *this;}

private:
    SwNode *head;
    SwNode *last;
    SwNode *findPrevNode(SwNode *n, HP::HPLocal hpLoc);
};





#endif /* SWLINKEDLIST_H_ */
