/*
 * NoFIFOPool.cpp
 *
 *  Created on: Dec 29, 2011
 */

#include "NoFIFOPool.h"
#include "../Configuration.h"
#include "../commons.h"
#include "../Atomic.h"
#include "SPChunk.h"
#include <assert.h>
#include "hp/hp.h"

using namespace HP;

NoFIFOPool::NoFIFOPool(int _numProducers, int _consumerID) :
	SCTaskPool(_numProducers), consumerID(_consumerID), currentNode(NULL),
			currentQueueID(-1) {

	int initialPoolSize;
	if (!Configuration::getInstance()->getVal(initialPoolSize,
			"initialPoolSize")) {
		initialPoolSize = 200;
	}

	chunkPool = new ChunkPool(consumerID, initialPoolSize);
	chunkLists = new SwLinkedList[_numProducers + 1];
	chunkListSizes = new unsigned int[numProducers + 1];
	reclaimChunkFunc = new ReclaimChunkFunc(chunkPool);

}

NoFIFOPool::~NoFIFOPool() {
	delete chunkPool;
	delete[] chunkLists;
	delete[] chunkListSizes;
	delete reclaimChunkFunc;
}

//TODO: implement:
SCTaskPool::ProducerContext* NoFIFOPool::getProducerContext(
		const Producer& prod) {
}

float NoFIFOPool::getStealingScore() const {
}
float NoFIFOPool::getStealingThreshold() const {
}

OpResult NoFIFOPool::consume(const Task*& t) {
	if (currentNode != NULL) {
		const Task* res = takeTask(currentNode);
		if (res != NULL) {
			t = res;
			return SUCCESS;
		}
	}

	//Go over the chunks list in a fair way
	int prevQueueIdx = currentQueueID;

	currentQueueID = (currentQueueID + 1) % numProducers;
	SwLinkedList::SwLinkedListIterator iter(NULL);
	while (currentQueueID != prevQueueIdx) {
		if (chunkListSizes[currentQueueID] != 0) {
			SwLinkedList currList = chunkLists[currentQueueID];
			iter.reset(&currList);
			SwNode* n = NULL;
			OpResult res;
			while ((res = iter.next(n)) != FAILURE && n != NULL) {
				const Task* res = takeTask(n);
				if (res != NULL) {
					t = res;
					currentNode = n;
					return SUCCESS;
				}
			}
			if (res == FAILURE)
				continue;
		}
		currentQueueID = (currentQueueID + 1) % numProducers;
	}

	currentNode = NULL;
	t = NULL;
	return EMPTY;

}

const Task* NoFIFOPool::steal(NoFIFOPool& from) {
	// gest a non-empty node or NULL if there are no nodes.
	int stealQueueID = -1;
	SwNode* prevNode = from.getStealNode(stealQueueID);
	if (prevNode == NULL)
		return NULL;
	SPChunk* c = prevNode->chunk;
	if (c == NULL)
		return NULL;
	//adding prevNode, so it would be stealble after ownership change.
	SwLinkedList stealList = chunkLists[numProducers];
	stealList.append(prevNode);
	if (c->changeOwner(from.consumerID, this->consumerID) == false) {
		stealList.remove(prevNode);
		return NULL;
	}
	SwNode* newNode = new SwNode(c);
	newNode->consumerIdx = prevNode->consumerIdx;
	stealList.replace(prevNode, newNode);
	prevNode->chunk = NULL;
	//update counters for stealer and the one we stole from
	FAA(&(chunkListSizes[numProducers]), 1);
	FAA(&(from.chunkListSizes[stealQueueID]), -1);

	int idx = newNode->consumerIdx;
	const Task* task = NULL;
	c->getTask(task, idx + 1);
	if (task == NULL)
		return NULL;
	if (task == TAKEN || !c->markTaken(idx + 1, task)) {
		task = NULL;
	} else if (idx + 2 == c->getMaxSize()) {
		reclaimChunk(newNode, c, numProducers);
	}
	newNode->consumerIdx++;
	return task;

}

int NoFIFOPool::getEmptynessCounter() const {
}

NoFIFOPool::ProdCtx::ProdCtx(SwLinkedList& l, unsigned int& c,
		NoFIFOPool& _noFIFOPool, int _producerId) :
	chunkList(l), chunkCount(c), curChunk(NULL), noFIFOPool(_noFIFOPool),
			producerId(_producerId) {
	curChunk = noFIFOPool.chunkPool->getChunk();
	chunkList.append(curChunk);
	FAA(&chunkCount, 1);
}

OpResult NoFIFOPool::ProdCtx::produce(const Task*& t, bool& changeConsumer) {
	return produceAux(t, changeConsumer);
}

void NoFIFOPool::ProdCtx::produceForce(const Task*& t) {
	bool dummy;
	produceAux(t, dummy, true);
}

OpResult NoFIFOPool::ProdCtx::produceAux(const Task*& t, bool& changeConsumer,
		bool force) {

	if (curChunk == NULL) {
		// the previous chunk is full
		// Try to get a new chunk from the chunk pool
		SPChunk* newChunk = noFIFOPool.chunkPool->getChunk();
		if (newChunk == NULL) {
			// no free chunks in the pool
			if (!force) {
				return FULL;
			} else {
				newChunk = new SPChunk(producerId);
			}
		}
		chunkList.append(newChunk);
		FAA(&chunkCount, 1);
		curChunk = newChunk;
		assert(curChunk->insertTask(t) == SUCCESS); // cannot fail
	}

	OpResult res = curChunk->insertTask(t);
	if (res == SUCCESS) {
		return SUCCESS;
	}
}

//AUX
void NoFIFOPool::reclaimChunk(SwNode *& n, SPChunk*& c, int QueueID) {
	HPLocal hpLoc = getHPLocal();
	n->chunk = NULL;
	currentNode = NULL;
	if (c->getOwner() == consumerID)
		FAA(&(chunkListSizes[QueueID]), -1);
	retireNode(c, reclaimChunkFunc, hpLoc);
}

const Task* NoFIFOPool::takeTask(SwNode* n) {
	HPLocal hpLoc = getHPLocal();
	SPChunk* chunk = n->chunk;
	setHP(3, chunk, hpLoc);
	if (n->chunk != chunk || chunk == NULL)
		return NULL;
	const Task* task = NULL;
	chunk->getTask(task, n->consumerIdx + 1);
	if (task == NULL)
		return NULL;

	n->consumerIdx++;
	if (chunk->getOwner() == consumerID) {
		//fast path:
		chunk->markTaken(n->consumerIdx);
		if (n->consumerIdx + 1 == chunk->getMaxSize()) {
			reclaimChunk(n, chunk, currentQueueID);
		}
		return task;
	}
	//chunk was stolen:
	FAA(&(chunkListSizes[currentQueueID]), -1);
	bool success = (task != TAKEN && chunk->markTaken(n->consumerIdx, task)
			== SUCCESS);
	n->chunk = NULL;
	currentNode = NULL;
	return success ? task : NULL;

}

// Gets a non-empty node from the longest list or NULL if no node exists
// prefer a non-stolen node, unless no list is bigger than 1.
// HP #2 will point to that node SP 3 will point to the chunk.
//TODO: check if this is a good way to choose chunks
SwNode* NoFIFOPool::getStealNode(int &stealQueueID) {
	SwLinkedList::SwLinkedListIterator iter(NULL);
	SwNode *n1 = NULL, *n2 = NULL;
	HPLocal hpLoc = getHPLocal();
	SPChunk* c = NULL;
	while (true) {
		//find list with max size:
		unsigned int max = 0;
		int maxIdx = -1;
		for (int i = 0; i < numProducers; i++) {
			if (chunkListSizes[i] > max) {
				max = chunkListSizes[i];
				maxIdx = i;
			}
		}

		if (max < 1 && chunkListSizes[numProducers] != 0) {
			stealQueueID = numProducers;
			//no list with more than 1 chunk - try to steal any node from steal list
			SwLinkedList list = chunkLists[numProducers];
			iter.reset(&list);
			if (iter.next(n1) == FAILURE)
				continue;
			if (n1 != NULL) {
				c = n1->chunk;
				setHP(3, c, hpLoc);
				if (n1->chunk != c)
					continue;
				if (c != NULL && c->hasTask(n1->consumerIdx + 1)) {
					setHP(2, n1, hpLoc);
					return n1;
				}
			}
		}
		if (max == 0)
			return NULL;

		// steal from longest list, try to steal the second chunk.
		// steal the first chunk if there is no second chunk.
		SwLinkedList list = chunkLists[maxIdx];
		stealQueueID = maxIdx;

		iter.reset(&list);
		if (iter.next(n1) == FAILURE)
			continue;
		//TODO: may just return NULL here:
		if (n1 == NULL)
			continue;
		setHP(2, n1, hpLoc);
		if (iter.next(n2) == FAILURE)
			continue;
		if (n2 != NULL) {
			c = n2->chunk;
			setHP(3, c, hpLoc);
			if (n2->chunk != c)
				continue;
			if (c != NULL && c->hasTask(n2->consumerIdx + 1)) {
				setHP(2, n2, hpLoc);
				return n2;
			}
		}
		c = n1->chunk;
		setHP(3, c, hpLoc);
		if (n1->chunk != c)
			continue;
		if (c != NULL && c->hasTask(n1->consumerIdx + 1)) {
			return n1;
		}
	}
}
