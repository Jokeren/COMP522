/*
 * ChunkPool.h
 *
 *  Created on: Dec 29, 2011
 *      Author: dima39
 */

#ifndef CHUNKPOOL_H_
#define CHUNKPOOL_H_

#include "SPChunk.h"
#include "MSQueue.hpp"
#include "AtomicStatistics.h"

class ChunkPool {
public:
	ChunkPool(int owner, int initialSize);
	virtual ~ChunkPool();

	SPChunk* getChunk(); // returns NULL if no chunk available
	void putChunk(SPChunk* c);
	int getOwner() const;

protected:
	int owner;
	AtomicStatistics* atomicStat;
	MSQueue<SPChunk*>* chunkQueue;
};

#endif /* CHUNKPOOL_H_ */
