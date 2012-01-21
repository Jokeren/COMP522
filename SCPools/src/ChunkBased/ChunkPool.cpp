/*
 * ChunkPool.cpp
 *
 *  Created on: Dec 29, 2011
 *      Author: dima39
 */

#include "ChunkPool.h"

ChunkPool::ChunkPool(int o, int initialSize) :
	owner(o),
	chunkQueue(new MSQueue<SPChunk*>())
{
	AtomicStatistics* stat = new AtomicStatistics(); // no need to count the initialization operations
	for(int i = 0; i < initialSize; i++) {
		putChunk(new SPChunk(owner), stat);
	}
	delete stat;
}

ChunkPool::~ChunkPool() {
	SPChunk* c;
	AtomicStatistics* stat = new AtomicStatistics();
	while ((c  = getChunk(stat)) != NULL)
		delete c;
	delete stat;
	delete chunkQueue;
}

SPChunk* ChunkPool::getChunk(AtomicStatistics* stat) {
	SPChunk* c;
	bool res = chunkQueue->dequeue(c, stat);
	if (res) {
		c->setOwner(owner);
		c->clean();
		return c;
	}
	return NULL;
}

void ChunkPool::putChunk(SPChunk* c, AtomicStatistics* stat) {
	this->chunkQueue->enqueue(c, stat);
}

int ChunkPool::getOwner() const {
	return this->owner;
}

