
#include <assert.h>

#include "NoFIFOPool.h"

#include "hp/hp.h"
using namespace HP;

#include <iostream>
using namespace std;

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
	return 3; // in the list with three chunks, there is hopefully at least one full chunk no one is working with
}


// Gets a non-empty node from the longest list or NULL if no node exists
// prefer a non-stolen node, unless no list is bigger than 1.
// HP #2 will point to that node SP 3 will point to the chunk.
//TODO: check if this is a good way to choose chunks
SwNode* NoFIFOPool::getStealNode(int &stealQueueID) {
	int idx = getLongestListIdx();
	if (chunkListSizes[idx] < getStealingThreshold()) return NULL;
	HPLocal hpLoc = getHPLocal();

	stealQueueID = idx;

	//todo: check if chunk is empty...
	SwNode* resNode = chunkLists[idx].getLast(hpLoc);

	static int count = 0;
	cout << "stole: " << count++ << endl;
	return resNode;


//	// take the second chunk from the beginning
//	while(true) {
//		SwLinkedList::SwLinkedListIterator iter(&(chunkLists[idx]));
//		SwNode* resNode;
//
//		OpResult res = iter.next(resNode); // the first chunk
//		if (res == FAILURE) continue;
//		if (resNode == NULL) return NULL;
//
//		res = iter.next(resNode); // the second chunk
//		if (res == FAILURE) continue;
//		setHP(2, resNode, hpLoc);
//		stealQueueID = idx;
//		return resNode;
//	}

}

Task* NoFIFOPool::steal(SCTaskPool* from_, AtomicStatistics* stat) {
	HPLocal hpLoc = getHPLocal();

	NoFIFOPool* from = (NoFIFOPool*)from_;
	// obtain a non-empty node or NULL if there are no nodes.
	int stealQueueID = -1;
	SwNode* const prevNode = from->getStealNode(stealQueueID);
	if (prevNode == NULL)
		return NULL;
	SPChunk* c = prevNode->chunk;
	setHP(3,c,hpLoc);
	if (c == NULL || c != prevNode->chunk)
		return NULL;

	// make the prevNode restealable by adding it to my own stealList
	SwLinkedList& stealList = chunkLists[numProducers];
	stealList.append(prevNode);
	// try to change the owner
	int prevOwner = c->getCountedOwner();
	if (SPChunk::getOwner(prevOwner) != from->consumerID ||
			(c->changeCountedOwner(prevOwner, this->consumerID) == false)) {
		// didn't succeed to change the owner (either it didn't correspond to the id of the from list
		// or it has been already stolen by someone else
		stealList.remove(prevNode);
		return NULL;
	}

	// succeeded to steal, create the copy of the node and put it in the stealing list
	SwNode* newNode = new SwNode(c);
	newNode->consumerIdx = prevNode->consumerIdx;
	if (newNode->consumerIdx +1 == c->getMaxSize()) {
		//chunk is empty - can't take a task
		delete newNode;
		stealList.remove(prevNode);
		return NULL;

	}
	stealList.replace(prevNode, newNode);

	//stealList.append(newNode);
	assert(prevNode->chunk == NULL || prevNode->chunk == c);
	prevNode->chunk = NULL;

	// update counters for stealer and the one we stole from
	FAA(&(chunkListSizes[numProducers]), 1);
	FAS(&(from->chunkListSizes[stealQueueID]), 1);

	// try to grab the task from the stolen chunk (in order to guarantee progress)
	int idx = newNode->consumerIdx;
	Task* task = NULL;
	c->getTask(task, idx + 1);
	if (task == NULL) return NULL;
	if (task == TAKEN || !c->markTaken(idx + 1, task)) {
		task = NULL;
	} else if (idx + 2 == c->getMaxSize()) {
		// stolen the last task in the chunk
		reclaimChunk(newNode, c, numProducers);
	}
	newNode->consumerIdx++;
	currentNode = newNode;
	return task;
}

