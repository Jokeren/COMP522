
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

	return chunkLists[idx].getLast(getHPLocal());
//
//	int chunkIdx = ((stealCounter++) % chunkListSizes[idx]);
//	// go and get the node with the chunkIdx
//	SwNode* resNode = NULL;
//	SwNode* prevNode = NULL;
//	SwLinkedList::SwLinkedListIterator iter(&(chunkLists[idx]));
//	while(true) {
//		bool iterOk = true;
//		iter.reset(&chunkLists[idx]);
//
//		for(int i = 0; i < chunkIdx; i++) {
//			if (iter.next(resNode) == SUCCESS) {
//				if (resNode == NULL) return prevNode; // the list is over :(
//			} else {
//				iterOk = false;
//				break;
//			}
//			prevNode = resNode;
//			setHP(2, prevNode, hpLoc); // to assure the node will not be deleted
//		}
//		if (iterOk)
//			return resNode;
//	}
}

//unsigned int noNodeFound = 0;
//unsigned int nodeWithoutChunk = 0;
//unsigned int failedChangeOwner = 0;
//unsigned int emptyChunkStolen = 0;

Task* NoFIFOPool::steal(SCTaskPool* from_, AtomicStatistics* stat) {
	HPLocal hpLoc = getHPLocal();

	NoFIFOPool* from = (NoFIFOPool*)from_;
	// obtain a non-empty node or NULL if there are no nodes.
	int stealQueueID = -1;
	SwNode* const prevNode = from->getStealNode(stealQueueID);
	if (prevNode == NULL) {
		//if ((++noNodeFound % 10000) == 0) cout << "noNodeFound " << noNodeFound << endl;
		return NULL;
	}

	SPChunk* c = prevNode->chunk;
	setHP(3,c,hpLoc);
	if (c == NULL || c != prevNode->chunk) {
		//if ((++nodeWithoutChunk % 10000) == 0) cout << "nodeWithoutChunk " << nodeWithoutChunk << endl;
		return NULL;
	}

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
		//if ((++failedChangeOwner % 10000) == 0) cout << "failedChangeOwner " << failedChangeOwner << endl;
		return NULL;
	}

	// succeeded to steal, create the copy of the node and put it in the stealing list
	SwNode* newNode = new SwNode(c);
	newNode->consumerIdx = prevNode->consumerIdx;
	if (newNode->consumerIdx +1 == c->getMaxSize()) {
		//chunk is empty - can't take a task
		delete newNode;
		stealList.remove(prevNode);
		//if ((++emptyChunkStolen % 10000) == 0) cout << "emptyChunkStolen " << emptyChunkStolen << endl;
		return NULL;

	}
	stealList.replace(prevNode, newNode);

	//stealList.append(newNode);
	assert(prevNode->chunk == NULL || prevNode->chunk == c);
	prevNode->chunk = NULL;

	// update counters for stealer and the one we stole from
	FAA(&(chunkListSizes[numProducers]), 1);
	while(true) {
		unsigned int curVal = from->chunkListSizes[stealQueueID];
		if (curVal == 0) break;
		if (CAS(&(from->chunkListSizes[stealQueueID]), curVal, curVal-1)) break;
	}
	//FAS(&(from->chunkListSizes[stealQueueID]), 1);

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

