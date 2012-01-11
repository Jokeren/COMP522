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

void deleteNode(void* node) {
	delete (SwNode*)node;
}


SwNode::SwNode(SPChunk* c) :
	chunk(c), consumerIdx(-1), next(0) {
}

SwLinkedList::SwLinkedList() {
	head = new SwNode(0);
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

// Get node in position NodeIdx,
// This is zero based, so the first node is at index zero.
// This method ignores nodes without chunks
// HP[0] will point to the returned node.

//SwNode *SwLinkedList::getNode(int NodeIdx) {
//	HPLocal hpLoc = getHPLocal();
//
//	while (true) {
//		//retry:
//		bool retry = false;
//		int currIdx = 0;
//		SwNode *currNode = head->next;
//		setHP(0, currNode, hpLoc);
//		if (currNode != head->next)
//			continue;
//
//		while (currIdx < NodeIdx && currNode != NULL) {
//			//ignore empty nodes
//			if (currNode->chunk != NULL) {
//				currIdx++;
//			}
//
//			Node* prev = currNode;
//			setHP(1, prev, hpLoc);
//			currNode = currNode->next;
//			setHP(0, currNode, hpLoc);
//			if (currNode != prev->next) {
//				retry = true;
//				break;
//			}
//
//		}
//		if (retry)
//			continue;
//		return (currIdx == NodeIdx) ? currNode : NULL;
//	}
//}



// removes the node from the list - assumes it is another list and doesn't need to be deleted
void SwLinkedList::remove(SwNode *nodeToRemove)
{
	SwNode* prev = findPrevNode(nodeToRemove);
	prev->next = nodeToRemove->next;
}

void SwLinkedList::replace(SwNode *nodeToReplace, SwNode *newNode)
{
	SwNode* prev = findPrevNode(nodeToReplace);
	assert(prev != NULL);
	newNode->next = nodeToReplace->next;
	prev->next = newNode;
}

SwNode* SwLinkedList::append(SPChunk *chunkToAdd) {
	SwNode* newNode = new SwNode(chunkToAdd);
	append(newNode);
	return newNode;
}

// No hazard pointers needed, since this is the only function
// that can remove nodes and only one thread may call it.

SwNode *SwLinkedList::findPrevNode(SwNode *n)
{
    SwNode *prev = head;
    SwNode *curr = head->next;
    HPLocal hpLoc = getHPLocal();
    while (curr != n && curr != NULL) {
		//remove empty nodes
		if (curr->chunk == NULL) {
			prev->next = curr->next;
			//so other threads won't go on:
			curr->next = NULL;
			retireNode(curr,deleteNode,hpLoc);
			curr = prev->next;
			continue;
		}
		prev = curr;
		curr = curr->next;
	}
    return (curr == n) ?  prev : NULL;
}

// Append a node to the list and remove unnecessary nodes
void SwLinkedList::append(SwNode *nodeToAdd) {
	SwNode* prev = findPrevNode(NULL);
	prev->next = nodeToAdd;
}


// uses HP 0 & 1
// Note: List function may not be used when using this iterator
// because they share the same HPs.

SwLinkedList::SwLinkedListIterator::SwLinkedListIterator(SwLinkedList* list) : hp0 (0), hp1(1) {
	curr = list->head;
	hpLoc = getHPLocal();

}

// skips removed nodes but doesn't release them
// may fail if current node was removed, in such a case reset should be called.
OpResult SwLinkedList::SwLinkedListIterator::next(SwNode*& node) {

	node = NULL;
	SwNode* prev  = curr;
	setHP(hp1,prev,hpLoc);

	curr = curr->next;
	setHP(hp0,curr,hpLoc);
	if (curr != prev->next) {
		curr = prev; //so it would fail again
		return FAILURE;
	}

	while (curr != NULL) {
		if (curr->chunk != NULL) {
			node = curr;
			if (curr != prev->next) return FAILURE;
			return SUCCESS;
		}
		prev = curr;
		std::swap(hp0,hp1);
		SwNode* next = curr->next;
		if (curr != prev->next) return FAILURE;
		curr = next;
		setHP(hp0,curr,hpLoc);
		if (curr != prev->next) {
			curr = prev; //so it would fail again
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
