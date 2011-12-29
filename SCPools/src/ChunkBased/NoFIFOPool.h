/*
 * NoFIFOPool.h
 *
 *  Created on: Dec 29, 2011
 *      Author: dima39
 */

#ifndef NOFIFOPOOL_H_
#define NOFIFOPOOL_H_

#include "../SCTaskPool.h"

class ChunkPool;
class LFLinkedList;

class NoFIFOPool : SCTaskPool {
public:
	class ProdCtx : SCTaskPool::ProducerContext {
	public:
		ProdCtx(LFLinkedList& l, unsigned int& count);
		virtual ~ProdCtx() {};
		virtual OpResult produce(const Task& t, bool& changeConsumer);
		virtual void produceForce(const Task& t);

	protected:
		virtual OpResult produceAux(const Task& t, bool& changeConsumer, bool force = false);
	private:
		LFLinkedList& chunkList;
		unsigned int& chunkCount;
		Chunk* curChunk;
	};

	NoFIFOPool(int _numProducers);
	virtual ~NoFIFOPool();

	virtual ProducerContext* getProducerContext(const Producer& prod);
	virtual OpResult consume(Task*& t);

	virtual float getStealingScore() const;
	virtual float getStealingThreshold() const;
	virtual Task* steal(SCTaskPool& from);

	virtual int getEmptynessCounter() const;

protected:
	ChunkPool* chunkPool;
};

#endif /* NOFIFOPOOL_H_ */
