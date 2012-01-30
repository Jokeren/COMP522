#include "SwedishPool.h"
#include "Atomic.h"
#include "Producer.h"

SwedishPool* SwedishPool::instance = NULL;
volatile bool SwedishPool::locked = false;

SwedishPool* SwedishPool::getInstance(int numProd) {
	if (instance == NULL) {
		// get the lock
		while(SwedishPool::locked || !CAS(&SwedishPool::locked, false, true));
		// the lock is ours
		if (instance == NULL) { // double check
			instance = new SwedishPool(numProd, -1);
			instance->setAtomicStatistics(new AtomicStatistics());
		}
		// free the lock
		SwedishPool::locked = false;
	}
	return instance;
}

SwedishPool* SwedishPool::getInstance() {
	return instance;
}

SwedishPool::SwedishPool(int _numProducers, int consId, bool createSingleton)
: NoFIFOCASPool(_numProducers, consId)
{
	SwedishPool::getInstance(_numProducers);
	lastStealingChunk = NULL;
	lastStealingIdx = -1;
	lastQueueId = consumerID % _numProducers;
	lastStealingNode = NULL;
	deleteChunkFunc = new DeleteChunkFunc();
};

SwedishPool::SwedishPool(int _numProducers, int consId)
: NoFIFOCASPool(_numProducers, consId)
{
	lastStealingChunk = NULL;
	lastStealingIdx = -1;
	lastQueueId = consumerID % _numProducers;
	lastStealingNode = NULL;
	deleteChunkFunc = new DeleteChunkFunc();
};


SCTaskPool::ProducerContext* SwedishPool::getProducerContext(const Producer& prod) {
	// assuming that producer ids start from 0
	// (this is the way the producers are created at Main.cpp)
	SwedishPool::getInstance()->prodContexts[prod.getId()]->setHP();
	return SwedishPool::getInstance()->prodContexts[prod.getId()];
}

Task* SwedishPool::steal(SCTaskPool* from, AtomicStatistics* stat) {
	if (lastStealingChunk != NULL) {
		Task* res = stealFromChunk(lastStealingChunk, lastStealingNode, lastQueueId, lastStealingIdx, stat);
		if (res != NULL) return res;
		lastStealingChunk = NULL; // next time go to the next chunk
	}

	// should always steal from SwedishPool::getInstance() (all the tasks are inserted to there)
	int idx = lastQueueId;
	for(int i = 0; i < numProducers; i++) {
		Task* res = stealFromList(&(SwedishPool::getInstance()->chunkLists[idx]), idx, stat);
		if (res != NULL) {
			return res;
		}
		idx = (idx + 1) % numProducers;
	}
}

OpResult SwedishPool::consume(Task*& t, AtomicStatistics* stat) {
	// a consumer don't have its own tasks
	Task* res = steal(SwedishPool::getInstance(), stat);
	t = res;
	return (t != NULL) ? SUCCESS : EMPTY;
}

Task* SwedishPool::stealFromChunk(SPChunk* chunk, SwNode* node, int queueId, int startIdx, AtomicStatistics* stat) {
	int idx = startIdx + 1;
	Task* t;
	while(true) {
		// iterate to the end of TAKEN values
		for(; idx < chunk->getMaxSize() && chunk->isTaken(idx); idx++) {};
		if (idx >= chunk->getMaxSize()) return NULL; // arrived to the end of the chunk
		chunk->getTask(t, idx);
		if (t == NULL) return NULL; // still no tasks at that idx
		if (t == TAKEN) continue;
		if (chunk->markTaken(idx, t, stat)) {
			// succeeded to steal the task
			lastStealingChunk = chunk;
			lastQueueId = queueId;
			lastStealingNode = node;
			lastStealingIdx = idx;
			if (idx+1 == chunk->getMaxSize()) {
				// that is the last task taken from the chunk, reclaim it
				node->chunk = NULL;
				currentNode = NULL;
				FAS(&(SwedishPool::getInstance()->chunkListSizes[queueId]), 1); stat->FetchAndIncCount_inc();
				retireNode(chunk, deleteChunkFunc, hpLoc);
				//retireNode(chunk, SwedishPool::getInstance()->reclaimChunkFunc, hpLoc);
				lastStealingChunk = NULL;
			}
			return t;
		}
	}
}
