
#include <assert.h>

#include "NoFIFOPool.h"

#include "hp/hp.h"
using namespace HP;

#include <iostream>
using namespace std;

#define THRESHOLD 300

int NoFIFOPool::getLongestListIdx() const {
	unsigned int maxScore = 0;
	int idx = 0;
	for(int i = 0; i < numProducers; i++) {
		if (chunkListSizes[i] > maxScore) {
			maxScore = chunkListSizes[i];
			idx = i;
		}
	}
	return idx;
}


float NoFIFOPool::getStealingScore() const {
	int longestListIdx = getLongestListIdx();
	return chunkListSizes[longestListIdx];
}

float NoFIFOPool::getStealingThreshold() const {
	return THRESHOLD; // in the list with three chunks, there is hopefully at least one full chunk no one is working with
}


// Gets a non-empty node from the longest list or NULL if no node exists
// prefer a non-stolen node, unless no list is bigger than 1.
// HP #2 will point to that node SP 3 will point to the chunk.
//TODO: check if this is a good way to choose chunks
SwNode* NoFIFOPool::getStealNode(int &stealQueueID) {
	int idx = getLongestListIdx();
	stealQueueID = idx;

	return chunkLists[idx].getLast(hpLoc);
}

Task* NoFIFOPool::steal(SCTaskPool* from_, AtomicStatistics* stat) {
	NoFIFOPool* from = (NoFIFOPool*)from_;
	// obtain a non-empty node or NULL if there are no nodes.
	int stealQueueID = -1;
	SwNode* const prevNode = from->getStealNode(stealQueueID);
	setHP(2,prevNode,hpLoc);
	if (prevNode == NULL) {
		return NULL;
	}

	SPChunk* c = prevNode->chunk;
	setHP(3,c,hpLoc);
	if (c == NULL || c != prevNode->chunk) {
		return NULL;
	}

	// make the prevNode restealable by adding it to my own stealList
	SwLinkedList& stealList = chunkLists[numProducers];
	stealList.append(prevNode, hpLoc);
	// try to change the owner
	int prevOwner = c->getCountedOwner();
	if (SPChunk::getOwner(prevOwner) != from->consumerID ||
			(c->changeCountedOwner(prevOwner, this->consumerID, stat) == false)) {
		// didn't succeed to change the owner (either it didn't correspond to the id of the from list
		// or it has been already stolen by someone else
		stealList.remove(prevNode, hpLoc);
		return NULL;
	}

	// succeeded to steal, create the copy of the node and put it in the stealing list
	SwNode* newNode = new SwNode(c);
	newNode->consumerIdx = prevNode->consumerIdx;
	if (newNode->consumerIdx +1 == c->getMaxSize()) {
		//chunk is empty - can't take a task
		delete newNode;
		stealList.remove(prevNode, hpLoc);
		return NULL;
	}
	stealList.replace(prevNode, newNode, hpLoc);

	//stealList.append(newNode);
	assert(prevNode->chunk == NULL || prevNode->chunk == c);
	prevNode->chunk = NULL;

	// update counters for stealer and the one we stole from
	FAA(&(chunkListSizes[numProducers]), 1); stat->FetchAndIncCount_inc();

	assert(newNode->consumerIdx +1 < c->getMaxSize() && newNode->consumerIdx == prevNode->consumerIdx);
	FAS(&(from->chunkListSizes[stealQueueID]), 1); stat->FetchAndIncCount_inc();
	// try to grab the task from the stolen chunk (in order to guarantee progress)
	int idx = newNode->consumerIdx;
	Task* task = NULL;
	c->getTask(task, idx + 1);
	if (task == NULL) return NULL;
	if (task == TAKEN || !c->markTaken(idx + 1, task, stat)) {
		task = NULL;
	} else if (idx + 2 == c->getMaxSize()) {
		// stolen the last task in the chunk
		reclaimChunk(newNode, c, numProducers, stat);
	}
	newNode->consumerIdx++;
	currentQueueID = numProducers;
	currentNode = newNode;
	return task;
}

