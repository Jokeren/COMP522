/*
 * Chunk.h
 *
 *  Created on: Dec 29, 2011
 *      Author: dima39
 */

#ifndef CHUNK_H_
#define CHUNK_H_

#include "../Task.h"
#include "../commons.h"

class Chunk {
public:
	Chunk(int owner);
	virtual ~Chunk();

	virtual OpResult insertTask(const Task& t, bool& isLastTask) = 0;
	virtual OpResult getTask(Task*& t, int idx) = 0;
	virtual OpResult markTaken(int idx, bool isCAS) = 0;
	virtual int getMaxSize() = 0;

	virtual int getOwner() const;
	virtual bool changeOwner(int prevOwner, int newOwner);

protected:
	int owner;
};

class ChunkFactory {
public:
	virtual Chunk* createChunk(int owner) = 0;
};

#endif /* CHUNK_H_ */
