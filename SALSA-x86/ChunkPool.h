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

	SPChunk* getChunk(AtomicStatistics* stat); // returns NULL if no chunk available
	void putChunk(SPChunk* c, AtomicStatistics* stat);
	int getOwner() const;

protected:
	int owner;
	MSQueue<SPChunk*>* chunkQueue;
};

#endif /* CHUNKPOOL_H_ */
