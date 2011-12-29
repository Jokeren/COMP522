/*
 * ChunkPool.h
 *
 *  Created on: Dec 29, 2011
 *      Author: dima39
 */

#ifndef CHUNKPOOL_H_
#define CHUNKPOOL_H_

#include "Chunk.h"

class ChunkPool {
public:
	ChunkPool(int initialSize, const ChunkFactory& factory);
	virtual ~ChunkPool();

	ChunkFactory& getChunkFactory();

	Chunk* getChunk(); // returns NULL if no chunk available
	void putChunk(Chunk* c);
};

#endif /* CHUNKPOOL_H_ */
