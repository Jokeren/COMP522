/*
 * NoFIFOPool.cpp
 *
 *  Created on: Dec 29, 2011
 */

#include "NoFIFOPool.h"
#include "Producer.h"
#include "Configuration.h"
#include "commons.h"
#include "Atomic.h"
#include "SPChunk.h"
#include <assert.h>
#include "hp/hp.h"

using namespace HP;

NoFIFOPool::NoFIFOPool(int _numProducers, int _consumerID) :
	SCTaskPool(_numProducers),
	consumerID(_consumerID),
	currentNode(NULL),
	currentQueueID(0)
{

	int initialPoolSize;
	if (!Configuration::getInstance()->getVal(initialPoolSize, "initialPoolSize")) {
		initialPoolSize = 200;
	}

	chunkPool = new ChunkPool(consumerID, initialPoolSize);
	chunkLists = new SwLinkedList[_numProducers + 1];
	chunkListSizes = new unsigned int[numProducers + 1];
	reclaimChunkFunc = new ReclaimChunkFunc(chunkPool);

	// create and initiate the contexts for every possible producer
	prodContexts = new ProdCtx*[_numProducers];
	for(int i = 0; i < _numProducers; i++) {
		prodContexts[i] = new ProdCtx(chunkLists[i], chunkListSizes[i], *chunkPool);
	}
}

NoFIFOPool::~NoFIFOPool() {
	delete chunkPool;
	delete[] chunkLists;
	delete[] chunkListSizes;
	delete reclaimChunkFunc;
	for(int i = 0; i < numProducers; i++) {
		delete prodContexts[i];
	}
	delete[] prodContexts;
}

SCTaskPool::ProducerContext* NoFIFOPool::getProducerContext(const Producer& prod) {
	// assuming that producer ids start from 0
	// (this is the way the producers are created at Main.cpp)
	return prodContexts[prod.getId()];
}

OpResult NoFIFOPool::consume(Task*& t, AtomicStatistics* stat) {
	if (currentNode != NULL) { // common case
		Task* res = takeTask(currentNode);
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
			// according to the counter, there are chunks in this list
			SwLinkedList currList = chunkLists[currentQueueID];
			iter.reset(&currList);
			SwNode* n = NULL;
			OpResult iterationStatus;
			while ((iterationStatus = iter.next(n)) != FAILURE && n != NULL) {
				Task* resTask = takeTask(n);
				if (resTask != NULL) {
					t = resTask;
					currentNode = n;
					return SUCCESS;
				}
			}

			// in case of concurrent list update, we want to start
			// iterating the list from the beginning
			if (iterationStatus == FAILURE)
				continue;
		}
		currentQueueID = (currentQueueID + 1) % numProducers;
	}

	// nothing helped -- we didn't find tasks
	currentNode = NULL;
	t = NULL;
	return EMPTY;

}

Task* NoFIFOPool::takeTask(SwNode* n) {
	HPLocal hpLoc = getHPLocal();
	SPChunk* chunk = n->chunk;
	setHP(3, chunk, hpLoc);
	if (n->chunk != chunk || chunk == NULL)
		return NULL;
	Task* task = NULL;
	chunk->getTask(task, n->consumerIdx + 1);
	if (task == NULL)
		return NULL;

	n->consumerIdx++;
	if (SPChunk::getOwner(chunk->getCountedOwner()) == consumerID) { //fast path:
		chunk->markTaken(n->consumerIdx);
		if (n->consumerIdx + 1 == chunk->getMaxSize()) {
			reclaimChunk(n, chunk, currentQueueID);
		}
		return task;
	}
	// the chunk was stolen:
	FAA(&(chunkListSizes[currentQueueID]), -1);
	bool success = (task != TAKEN &&
			(chunk->markTaken(n->consumerIdx, task) == SUCCESS));
	n->chunk = NULL;
	currentNode = NULL;
	return success ? task : NULL;
}


int NoFIFOPool::getEmptynessCounter() const {
	return 0; // TODO should probably be something more complicated :)
}


void NoFIFOPool::reclaimChunk(SwNode* n, SPChunk* c, int QueueID) {
	HPLocal hpLoc = getHPLocal();
	n->chunk = NULL;
	currentNode = NULL;
	if (SPChunk::getOwner(c->getCountedOwner()) == consumerID) {
		FAA(&(chunkListSizes[QueueID]), -1);
	}
	retireNode(c, reclaimChunkFunc, hpLoc);
}


