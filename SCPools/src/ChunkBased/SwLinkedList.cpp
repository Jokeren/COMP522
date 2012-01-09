/*
 * SwLinkedList.cpp
 *
 *  Created on: Jan 8, 2012
 *      Author: elad
 */

#include "SwLinkedList.h"

//TODO: insert hazard pointers

SwNode::SwNode(Chunk* c) : chunk(c), consumerIdx(-1), next(0){}

SwLinkedList::SwLinkedList() {
	head = new SwNode(0);
}

//Assumes that no other thread is pointing to nodes.
SwLinkedList::~SwLinkedList() {
	SwNode* curr = head;

	while(curr != NULL) {
		SwNode* tmp = curr;
		curr = curr->next;
		delete tmp;
	}

}

// Get node in position NodeIdx,
// This is zero based, so the first node is at index zero.
// This method ignores nodes without chunksץ

SwNode *SwLinkedList::getNode(int NodeIdx) {
	int currIdx = 0;
	SwNode *currNode = head->next;

	while (currIdx < NodeIdx && currNode != NULL) {
		//ignore empty nodes
		if (currNode->chunk != NULL) {
			currIdx++;
		}
		currNode = currNode->next;
	}
	return (currIdx == NodeIdx) ? currNode : NULL;
}

SwNode* SwLinkedList::append(Chunk *chunkToAdd) {
	SwNode* newNode = new SwNode(chunkToAdd);
	append(newNode);
	return newNode;
}

void SwLinkedList::append(SwNode *nodeToAdd)
{
	SwNode* prev = head;
	SwNode* curr = head->next;

	while (curr != NULL) {
		//remove empty nodes
		if (curr->chunk == NULL) {
			prev->next = curr->next;
			delete curr;
			curr = prev->next;
			continue;
		}
		prev = curr;
		curr = curr->next;
	}
	prev->next = nodeToAdd;
}

SwLinkedList::SwLinkedListIterator::SwLinkedListIterator(SwLinkedList* list) {
	curr = list->head;
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
