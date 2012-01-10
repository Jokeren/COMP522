/*
 * SwLinkedList.cpp
 *
 *  Created on: Jan 8, 2012
 *      Author: elad
 */

#include "SwLinkedList.h"

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

SwNode* SwLinkedList::append(SPChunk *chunkToAdd) {
	SwNode* newNode = new SwNode(chunkToAdd);
	append(newNode);
	return newNode;
}

// Append a node to the list and remove unnecessary nodes
// No hazard pointers needed, since this is the only function
// that can remove nodes and only one thread may call it.

// HP 0 will point to the new node when returned
void SwLinkedList::append(SwNode *nodeToAdd) {
	SwNode* prev = head;
	SwNode* curr = head->next;

	HPLocal hpLoc = getHPLocal();

	while (curr != NULL) {
		//remove empty nodes
		if (curr->chunk == NULL) {
			prev->next = curr->next;
			retireNode(curr,deleteNode,hpLoc);
			curr = prev->next;
			continue;
		}
		prev = curr;
		curr = curr->next;
	}
	setHP(0,nodeToAdd,hpLoc);
	prev->next = nodeToAdd;
}


// uses HP 0 & 1
// Note: List function may not be used when using this iterator
// because they share the same HPs.

SwLinkedList::SwLinkedListIterator::SwLinkedListIterator(SwLinkedList* list) {
	curr = list->head;
	hpLoc = getHPLocal();
}

//skips removed nodes but doesn't release them
SwNode* SwLinkedList::SwLinkedListIterator::next() {
	curr = curr->next;
	while (curr != NULL) {
		if (curr->chunk != NULL) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}
//reset to head
void SwLinkedList::SwLinkedListIterator::reset(SwLinkedList* list) {
	curr = list->head;
}
