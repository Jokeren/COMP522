/*
 * NoFIFOPool.h
 *
 *  Created on: Dec 29, 2011
 *      Author: dima39
 */

#ifndef NOFIFOPOOL_H_
#define NOFIFOPOOL_H_

#include "../SCTaskPool.h"

#include "ChunkPool.h"
#include "SwLinkedList.h"
#include "SPChunk.h"

class NoFIFOPool : public SCTaskPool {

public:

	class ReclaimChunkFunc : public HP::ReclaimationFunc {

	public:
		ReclaimChunkFunc(ChunkPool* cp) : chunkPool(cp) {}
		void operator() (void* chunk) {
			((SPChunk*)chunk)->clean();
			chunkPool->putChunk((SPChunk*)chunk);
		}

	private:
		ChunkPool* chunkPool;
	};


	class ProdCtx : public SCTaskPool::ProducerContext {
	public:
		ProdCtx(SwLinkedList& l, unsigned int& count, ChunkPool& _chunkPool);
		virtual ~ProdCtx() {};

		virtual OpResult produce(Task& t, bool& changeConsumer, AtomicStatistics* stat);
		virtual void produceForce(Task& t, AtomicStatistics* stat);

	protected:
		virtual OpResult produceImpl(Task& t, bool& changeConsumer, bool force = false);
	private:
		SwLinkedList& chunkList;
		unsigned int& chunkCount;
		ChunkPool& chunkPool;
		SPChunk* curChunk;
	};

	NoFIFOPool(int _numProducers, int _consumerID);
	virtual ~NoFIFOPool();

	virtual SCTaskPool::ProducerContext* getProducerContext(const Producer& prod);
	virtual OpResult consume(Task*& t, AtomicStatistics* stat);

	virtual float getStealingScore() const;
	virtual float getStealingThreshold() const;
	virtual Task* steal(SCTaskPool* from, AtomicStatistics* stat);

	virtual int getEmptynessCounter() const;

protected:
	int consumerID;
	SwLinkedList* chunkLists;
	SwNode* currentNode;
	ChunkPool* chunkPool;
	unsigned int *chunkListSizes;
	int currentQueueID;
	ReclaimChunkFunc* reclaimChunkFunc;
	NoFIFOPool::ProdCtx** prodContexts;

protected:
	virtual Task* takeTask(SwNode* n);
	SwNode* getStealNode(int &stealQueueID);
	void reclaimChunk(SwNode* n, SPChunk* c, int QueueID);
};

#endif /* NOFIFOPOOL_H_ */
