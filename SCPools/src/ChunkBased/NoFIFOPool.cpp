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

NoFIFOPool::NoFIFOPool(int _numProducers) : SCTaskPool(_numProducers)
{

	int initialPoolSize;
	if (!Configuration::getInstance()->getVal(initialPoolSize, "initialPoolSize")) {
		initialPoolSize = 200;
	}

	chunkPool = new ChunkPool(initialPoolSize, *(new SPChunkFactory()));
}

NoFIFOPool::~NoFIFOPool() {
	// TODO Auto-generated destructor stub
}



//TODO: implement:
SCTaskPool::ProducerContext* NoFIFOPool::getProducerContext(const Producer& prod) {}
OpResult NoFIFOPool::consume(Task*& t) {}

float NoFIFOPool::getStealingScore() const {}
float NoFIFOPool::getStealingThreshold() const {}
Task* NoFIFOPool::steal(SCTaskPool& from) {}

int NoFIFOPool::getEmptynessCounter() const {}




NoFIFOPool::ProdCtx::ProdCtx(LFLinkedList& l, unsigned int& c, NoFIFOPool& _noFIFOPool, int _producerId)
 : chunkList(l),
   chunkCount(c),
   curChunk(NULL),
   noFIFOPool(_noFIFOPool),
   producerId(_producerId)
{
	curChunk = noFIFOPool.chunkPool->getChunk();
	chunkList.append(curChunk);
	FAA(&chunkCount, 1);
}

OpResult NoFIFOPool::ProdCtx::produce(const Task& t, bool& changeConsumer) {
	return produceAux(t, changeConsumer);
}

void NoFIFOPool::ProdCtx::produceForce(const Task& t) {
	bool dummy;
	produceAux(t, dummy, true);
}

OpResult NoFIFOPool::ProdCtx::produceAux(const Task& t, bool& changeConsumer, bool force) {
	OpResult res = curChunk->insertTask(t, changeConsumer);
	if (res == SUCCESS) {
		return SUCCESS;
	}
	// the previous chunk is full
	// Try to get a new chunk from the chunk pool
	Chunk* newChunk = noFIFOPool.chunkPool->getChunk();
	if (newChunk == NULL) {
		// no free chunks in the pool
		if (!force)	{
			return FULL;
		} else {
			newChunk = noFIFOPool.chunkPool->getChunkFactory().createChunk(producerId);
		}
	}
	chunkList.append(newChunk);
	FAA(&chunkCount, 1);
	curChunk = newChunk;
	assert(curChunk->insertTask(t, changeConsumer) == SUCCESS); // cannot fail
	return SUCCESS;
}

//	NoFIFOPool(int _numProducers);
//	virtual ~NoFIFOPool();
//
//	virtual ProducerContext* getProducerContext(const Producer& prod);
//	virtual OpResult consume(Task*& t);
//
//	virtual float getStealingScore() const;
//	virtual float getStealingThreshold() const;
//	virtual Task* steal(SCTaskPool& from);
//
//	virtual int getEmptynessCounter() const;
