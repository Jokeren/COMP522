/*
 * SPChunk.h
 *
 *  Single producer chunk
 *  Created on: Dec 29, 2011
 */

#ifndef SPCHUNK_H_
#define SPCHUNK_H_

class SPChunk : Chunk {
public:
	SPChunk();
	virtual ~SPChunk();

	virtual OpResult insertTask(const Task& t);
	virtual OpResult getTask(Task*& t);

	virtual int getMaxSize();
};

class SPChunkFactory : ChunkFactory {
public:
	SPChunkFactory() {}
	SPChunk* createChunk() {
		return new SPChunk();
	}
};

#endif /* SPCHUNK_H_ */
