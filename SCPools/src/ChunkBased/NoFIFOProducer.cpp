#include "NoFIFOPool.h"

NoFIFOPool::ProdCtx::ProdCtx(SwLinkedList& l, unsigned int& c, ChunkPool& _chunkPool) :
	chunkList(l),
	chunkCount(c),
	chunkPool(_chunkPool),
	curChunk(NULL)
{
}

OpResult NoFIFOPool::ProdCtx::produce(Task& t, bool& changeConsumer, AtomicStatistics* stat) {
	return produceImpl(t, changeConsumer, false);
}

void NoFIFOPool::ProdCtx::produceForce(Task& t, AtomicStatistics* stat) {
	bool dummy;
	produceImpl(t, dummy, true);
}

OpResult NoFIFOPool::ProdCtx::produceImpl(Task& t, bool& changeConsumer,	bool force) {
	if (curChunk == NULL) {
		// The previous chunk is full
		// Try to get a new chunk from the chunk pool
		SPChunk* newChunk = chunkPool.getChunk();
		if (newChunk == NULL) {
			// no free chunks in the pool
			if (!force) {
				return FULL;
			} else {
				newChunk = new SPChunk(chunkPool.getOwner());
			}
		}
		chunkList.append(newChunk);
		FAA(&chunkCount, 1);
		curChunk = newChunk;
	}

	bool lastTask;
	curChunk->insertTask(&t, lastTask);
	if (lastTask) {
		curChunk = NULL; // the next produce operation will start a new chunk
	}
	return SUCCESS;
}

