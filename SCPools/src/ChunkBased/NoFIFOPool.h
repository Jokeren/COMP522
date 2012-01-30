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
		ReclaimChunkFunc(ChunkPool* cp) : chunkPool(cp), stat(NULL) {}
		void setAtomicStatistics(AtomicStatistics* _stat) {
			this->stat = _stat;
		}

		void operator() (void* chunk) {
			chunkPool->putChunk((SPChunk*)chunk, stat);
		}

	private:
		ChunkPool* chunkPool;
		AtomicStatistics* stat;
	};

	class DeleteChunkFunc : public HP::ReclaimationFunc {

	public:
		void operator() (void* chunk) {
			delete (SPChunk*)chunk;
		}
	};

	class ProdCtx : public SCTaskPool::ProducerContext {
	public:
		ProdCtx(SwLinkedList& l, unsigned int& count, ChunkPool& _chunkPool);
		virtual ~ProdCtx() {};

		virtual OpResult produce(Task& t, bool& changeConsumer, AtomicStatistics* stat);
		virtual void produceForce(Task& t, AtomicStatistics* stat);
		virtual void setHP();
		
	protected:
		virtual OpResult produceImpl(Task& t, bool& changeConsumer, bool force, AtomicStatistics* stat);
	private:
		SwLinkedList& chunkList;
		unsigned int& chunkCount;
		ChunkPool& chunkPool;
		SPChunk* curChunk;
		HP::HPLocal hpLoc;
	};

	NoFIFOPool(int _numProducers, int _consumerID);
	virtual ~NoFIFOPool();

	virtual SCTaskPool::ProducerContext* getProducerContext(const Producer& prod);
	virtual OpResult consume(Task*& t, AtomicStatistics* stat);

	virtual float getStealingScore() const;
	virtual float getStealingThreshold() const;
	virtual Task* steal(SCTaskPool* from, AtomicStatistics* stat);
	int getLongestListIdx() const;
	virtual int getEmptynessCounter() const;

	virtual void setAtomicStatistics(AtomicStatistics* stat) {
		this->reclaimChunkFunc->setAtomicStatistics(stat);
	};
protected:
	int consumerID;
	SwLinkedList* chunkLists;
	SwNode* currentNode;
	ChunkPool* chunkPool;
	unsigned int *chunkListSizes;
	int currentQueueID;
	ReclaimChunkFunc* reclaimChunkFunc;
	NoFIFOPool::ProdCtx** prodContexts;
	unsigned int stealCounter;
	HP::HPLocal hpLoc;
protected:
	virtual Task* takeTask(SwNode* n, AtomicStatistics* stat);
	SwNode* getStealNode(int &stealQueueID);
	void reclaimChunk(SwNode* n, SPChunk* c, int QueueID, AtomicStatistics* stat);
};

#endif /* NOFIFOPOOL_H_ */
