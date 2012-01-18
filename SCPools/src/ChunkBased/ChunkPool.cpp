/*
 * ChunkPool.cpp
 *
 *  Created on: Dec 29, 2011
 *      Author: dima39
 */

#include "ChunkPool.h"

ChunkPool::ChunkPool(int o, int initialSize) :
	owner(o),
	atomicStat(new AtomicStatistics()),
	chunkQueue(new MSQueue<SPChunk*>())
{
	for(int i = 0; i < initialSize; i++) {
		putChunk(new SPChunk(owner));
	}
}

ChunkPool::~ChunkPool() {
	SPChunk* c;
	while ((c  = getChunk()) != NULL)
		delete c;
	delete atomicStat;
	delete chunkQueue;
}

SPChunk* ChunkPool::getChunk() {
	SPChunk* c;
	bool res = chunkQueue->dequeue(c, atomicStat);
	if (res) {
		c->setOwner(owner);
		c->clean();
		return c;
	}
	return NULL;
}

void ChunkPool::putChunk(SPChunk* c) {
	this->chunkQueue->enqueue(c, atomicStat);
}

int ChunkPool::getOwner() const {
	return this->owner;
}

