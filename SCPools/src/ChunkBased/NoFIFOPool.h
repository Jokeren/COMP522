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
	class ProdCtx : SCTaskPool::ProducerContext {
	public:
		ProdCtx(SwLinkedList& l, unsigned int& count, NoFIFOPool& _noFIFOPool, int _producerId);
		virtual ~ProdCtx() {};
		virtual OpResult produce(const Task& t, bool& changeConsumer);
		virtual void produceForce(const Task& t);

	protected:
		virtual OpResult produceAux(const Task& t, bool& changeConsumer, bool force = false);
	private:
		SwLinkedList& chunkList;
		unsigned int& chunkCount;
		SPChunk* curChunk;
		NoFIFOPool& noFIFOPool;
		int producerId;
	};

	NoFIFOPool(int _numProducers, int _consumerID);
	virtual ~NoFIFOPool();

	virtual SCTaskPool::ProducerContext* getProducerContext(const Producer& prod);
	virtual OpResult consume(Task*& t);

	virtual float getStealingScore() const;
	virtual float getStealingThreshold() const;
	Task* steal(NoFIFOPool& from);

	virtual int getEmptynessCounter() const;

protected:
	int consumerID;
	SwLinkedList* chunkLists;
	SwNode* currentNode;
	ChunkPool* chunkPool;
	unsigned int *chunkListSizes;


	int currentQueueID;

private:
	Task* takeTask(SwNode* n);
	SwNode* getStealNode(int &stealQueueID);
	void reclaimChunk(SwNode *& n, SPChunk*& c, int QueueID);
};

#endif /* NOFIFOPOOL_H_ */
