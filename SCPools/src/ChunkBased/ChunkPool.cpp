/*
 * ChunkPool.cpp
 *
 *  Created on: Dec 29, 2011
 *      Author: dima39
 */

#include "ChunkPool.h"

ChunkPool::ChunkPool(int initialSize, const SPChunkFactory& factory) {
	// TODO Auto-generated constructor stub

}

ChunkPool::~ChunkPool() {
	// TODO Auto-generated destructor stub
}

//TODO: Implement
SPChunkFactory& ChunkPool::getChunkFactory() {}
SPChunk* ChunkPool::getChunk() {}
void ChunkPool::putChunk(SPChunk* c) {}

