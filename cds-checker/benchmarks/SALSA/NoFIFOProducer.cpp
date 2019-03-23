#include "NoFIFOPool.h"
#include "hp/hp.h"

NoFIFOPool::ProdCtx::ProdCtx(SwLinkedList& l, AtomicWrapper<int> &c, ChunkPool& _chunkPool) :
	chunkList(l),
	chunkCount(c),
	chunkPool(_chunkPool),
	curChunk(NULL),
	hpLoc(NULL)
{
}

OpResult NoFIFOPool::ProdCtx::produce(Task& t, bool& changeConsumer, AtomicStatistics* stat) {
	return produceImpl(t, changeConsumer, false, stat);
}

void NoFIFOPool::ProdCtx::produceForce(Task& t, AtomicStatistics* stat) {
	bool dummy;
	produceImpl(t, dummy, true, stat);
}

OpResult NoFIFOPool::ProdCtx::produceImpl(Task& t, bool& changeConsumer, bool force, AtomicStatistics* stat) {
	if (curChunk == NULL) {
		// The previous chunk is full
		// Try to get a new chunk from the chunk pool
		SPChunk* newChunk = chunkPool.getChunk(stat);
		if (newChunk == NULL) {
			// no free chunks in the pool
			if (!force) {
				return FULL;
			} else {
				newChunk = new SPChunk(chunkPool.getOwner());
			}
		}
		chunkCount.fetch_add(1); stat->FetchAndIncCount_inc();
		chunkList.append(newChunk, hpLoc);
		curChunk = newChunk;
	}

	bool lastTask;
	curChunk->insertTask(&t, lastTask);
	if (lastTask) {
		changeConsumer = true;
		curChunk = NULL; // the next produce operation will start a new chunk
	}
	return SUCCESS;
}

void NoFIFOPool::ProdCtx::setHP() {
	if (hpLoc == NULL) hpLoc = HP::getHPLocal();
}
