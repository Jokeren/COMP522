/*
 * SwLinkedList.cpp
 *
 *  Created on: Jan 8, 2012
 *      Author: elad
 */

#include "SwLinkedList.h"
#include <algorithm>
#include <assert.h>

using namespace HP;

class DeleteNode: public HP::ReclaimationFunc {

public:
	void operator()(void* node) {
		delete (SwNode*) node;
	}

	static DeleteNode* getInstance() {
		if (instance == NULL)
			instance = new DeleteNode();
		return instance;
	}
private:
	DeleteNode() {
	}
	static DeleteNode* instance;

};
DeleteNode* DeleteNode::instance = NULL;

SwNode::SwNode(SPChunk* c) :
	chunk(c), consumerIdx(-1), next(0) {
}

SwLinkedList::SwLinkedList() {
	head = new SwNode(0);
	last = head;
}

//Assumes that no other thread is pointing to nodes.
SwLinkedList::~SwLinkedList() {
	SwNode* curr = head;

	while (curr != NULL) {
		SwNode* tmp = curr;
		curr = curr->next;
		delete tmp;
	}
}

// removes the node from the list - assumes it is another list and doesn't need to be deleted
// Also assumes it was the last node in the current list so the previous node points to NULL
void SwLinkedList::remove(SwNode *nodeToRemove, HPLocal hpLoc) {
	SwNode* prev = findPrevNode(nodeToRemove, hpLoc);
	prev->next = NULL;
}
// replace a node from the list with another node - assumes it is another list and doesn't need to be deleted
// Also assumes it was the last node in the current list so the new node points to NULL
void SwLinkedList::replace(SwNode *nodeToReplace, SwNode *newNode, HPLocal hpLoc) {
	SwNode* pred = findPrevNode(nodeToReplace, hpLoc);
	newNode->next = NULL;
	pred->next = newNode;
	last = newNode;
}

SwNode* SwLinkedList::append(SPChunk *chunkToAdd, HPLocal hpLoc) {
	SwNode* newNode = new SwNode(chunkToAdd);
	append(newNode,hpLoc);
	last = newNode;
	return newNode;
}

// No hazard pointers needed, since this is the only function
// that can remove nodes and only one thread may call it.
SwNode *SwLinkedList::findPrevNode(SwNode *n, HPLocal hpLoc) {
	SwNode *prev = head;
	SwNode *curr = head->next;
	while (curr != n && curr != NULL) {
		//remove empty nodes
		if (curr->chunk == NULL) {
			prev->next = curr->next;
			if (prev->next == NULL)
				last = prev;
			//so other threads won't go on:
			curr->next = NULL;
			retireNode(curr, DeleteNode::getInstance(), hpLoc);
			curr = prev->next;
			continue;
		}
		prev = curr;
		curr = curr->next;
	}
	return (curr == n) ? prev : NULL;
}

// Append a node to the list and remove unnecessary nodes
void SwLinkedList::append(SwNode *nodeToAdd, HPLocal hpLoc) {
	SwNode* prev = findPrevNode(NULL, hpLoc);
	assert(prev->next == NULL);
	prev->next = nodeToAdd;
}

//gets last node as sets HP to it
SwNode* SwLinkedList::getLast(HPLocal hpLoc) {
	while (true) {
		SwNode* res = last;
		setHP(0, res, hpLoc);
		if (res != last)
			continue;
		return res;
	}
}

// uses HP 0 & 1
// Note: List function may not be used when using this iterator
// because they share the same HPs.

SwLinkedList::SwLinkedListIterator::SwLinkedListIterator(SwLinkedList* list, HPLocal _hpLoc) :
	hp0(0), hp1(1) {
	curr = (list == NULL) ? NULL : list->head;
	hpLoc = _hpLoc;
	setHP(hp0, curr, hpLoc);
}

// skips removed nodes but doesn't release them
// may fail if current node was removed, in such a case reset should be called.
OpResult SwLinkedList::SwLinkedListIterator::next(SwNode*& node) {
	node = NULL;
	SwNode* prev = curr;
	setHP(hp1, prev, hpLoc);

	curr = curr->next;
	setHP(hp0, curr, hpLoc);
	if (curr != prev->next) {
		curr = NULL; //so it would fail again
		return FAILURE;
	}

	while (curr != NULL) {
		if (curr->chunk != NULL) {
			node = curr;
			if (curr != prev->next)
				return FAILURE;
			return SUCCESS;
		}
		SwNode* next = curr->next;
		if (curr != prev->next)
			return FAILURE;
		prev = curr;
		std::swap(hp0, hp1);
		curr = next;
		setHP(hp0, curr, hpLoc);
		if (curr != prev->next) {
			curr = NULL; //so it would fail again
			return FAILURE;
		}
	}
	node = NULL;
	return SUCCESS;
}

//reset to head
void SwLinkedList::SwLinkedListIterator::reset(SwLinkedList* list) {
	curr = list->head;
}
