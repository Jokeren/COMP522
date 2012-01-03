/*
 * ChunkPool.cpp
 *
 *  Created on: Dec 29, 2011
 *      Author: dima39
 */

#include "ChunkPool.h"

ChunkPool::ChunkPool(int initialSize, const ChunkFactory& factory) {
	// TODO Auto-generated constructor stub

}

ChunkPool::~ChunkPool() {
	// TODO Auto-generated destructor stub
}

//TODO: Implement
ChunkFactory& ChunkPool::getChunkFactory() {}
Chunk* ChunkPool::getChunk() {}
void ChunkPool::putChunk(Chunk* c) {}

