/*
 * LinkedList.cpp
 *
 *  Created on: Dec 29, 2011
 *      Author: dima39
 */

#include "LinkedList.h"
#include <stdlib.h>


//Aux function deceleration:

void find(Node *head, void* toFind, int findIdx, Node*& predRes, Node*& currRes, HPLocal hpLoc);
void findNode(Node *head, void* toFind, Node*& predRes, Node*& currRes, HPLocal hpLoc);
void findIdx(Node *head, int findIdx, Node*& predRes, Node*& currRes, HPLocal hpLoc);

Node::Node(Chunk* c) : chunk(c), consIdx(0), next(0)
{
}

LFLinkedList::LFLinkedList() {
	//first and last nodes are sentinel nodes
	this->head = new Node(0);
	this->head->next = (markable_ref)new Node(0);
}

LFLinkedList::~LFLinkedList() {
	// TODO Auto-generated destructor stub
}



Node* LFLinkedList::append(Chunk* c, HPLocal hpLoc) {
	Node *pred, *curr;
	Node *newNode = new Node(c);
	while (true) {
		findNode(this->head, NULL, pred, curr,hpLoc);
		newNode->next = (markable_ref)curr;
		if (REF_CAS(&(pred->next), curr, newNode, FALSE_MARK, FALSE_MARK)) return newNode;
	}
}

Node* LFLinkedList::get(int idx, HPLocal hpLoc) {
	Node *pred;
	Node *curr;
	findIdx(this->head, idx, pred, curr, hpLoc);
	if (GET_REF(curr->next) == NULL) return NULL;
	return curr;
}
bool LFLinkedList::remove(Node* toRemove, HPLocal hpLoc) {
	int snip;
	Node *pred, *curr;
	while (true) {
		findNode(this->head, toRemove, pred, curr, hpLoc);
		if (curr != toRemove)
			return false;
		else {
			Node *succ = (Node*)GET_REF(curr->next);
			snip = REF_CAS(&(curr->next),succ,succ,FALSE_MARK,TRUE_MARK);
			if (!snip)
				continue;
			if (REF_CAS(&(pred->next),curr,succ,FALSE_MARK,FALSE_MARK))
				retireNode(curr,&free,hpLoc);
			return true;
		}
	}
}

//AUX

void find(Node *head, void* toFind, int findIdx, Node*& predRes, Node*& currRes, HPLocal hpLoc) {
	Node *pred = NULL, *curr = NULL, *succ = NULL;
	int marked = FALSE_MARK;
	int snip, retry = false;
	int currIdx = 0;

	int hpIdx0 = 0;
	int hpIdx1 = 1;

	while (true) {
		//retry:
		retry = false;
		pred = head;
		curr = (Node*)GET_REF(pred->next);

		setHP(hpIdx0,curr,hpLoc);
		if (curr != GET_REF(pred->next) || GET_MARK(pred->next)) continue;

		while (true) {
			succ = (Node*)get_mark_and_ref(curr->next, &marked);
			while (marked) {
				snip = REF_CAS(&(pred->next),curr,succ,FALSE_MARK,FALSE_MARK);
				if (!snip) {
					retry = true;
					break;
				}
				retireNode(curr,&free,hpLoc);
				curr = succ;

				setHP(hpIdx0,curr,hpLoc);
				if (curr != GET_REF(pred->next) || GET_MARK(pred->next)) {
					retry = true;
					break;
				}

				succ = (Node*)get_mark_and_ref(curr->next, &marked);
			}
			if (retry)
				break;
			if (curr == toFind || succ == NULL || findIdx == currIdx) {
				predRes = pred;
				currRes = curr;
				return;
			}
			currIdx++;
			pred = curr;
			curr = succ;
			//swap hp
			int tmp = hpIdx0;
			hpIdx0 = hpIdx1;
			hpIdx1 = tmp;
		}
		if (retry)
			continue;
	}
}

void findNode(Node *head, void* toFind, Node*& predRes, Node*& currRes, HPLocal hpLoc) {
	find(head, toFind, -1, predRes, currRes, hpLoc);
}
void findIdx(Node *head, int findIdx, Node*& predRes, Node*& currRes, HPLocal hpLoc) {
	find(head, NULL, findIdx, predRes, currRes, hpLoc);
}
