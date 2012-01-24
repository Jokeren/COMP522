#ifndef LIFOPOOL_H_
#define LIFOPOOL_H_

#include "lifo.h"

class LIFOProducerContext;

class LIFOPool : public SCTaskPool {
private:
	LIFOProducerContext* prodCtx;
	Lifo* stack;
	volatile unsigned int size;
	virtual void insert(Task* t);
public:
	LIFOPool();
	virtual ~LIFOPool();

	virtual ProducerContext* getProducerContext(const Producer& prod);
	virtual OpResult consume(Task*& t, AtomicStatistics* stat);

	virtual float getStealingScore() const;
	virtual float getStealingThreshold() const;
	virtual Task* steal(SCTaskPool* from, AtomicStatistics* stat);

	virtual int getEmptynessCounter() const;

	friend class LIFOProducerContext;
};

class LIFOProducerContext : public SCTaskPool::ProducerContext {
private:
	LIFOPool* pool;
public:
	LIFOProducerContext(LIFOPool* p);
	virtual OpResult produce(Task& t, bool& changeConsumer, AtomicStatistics* stat);
	virtual void produceForce(Task& t, AtomicStatistics* stat);
};



#endif /* LIFOPOOL_H_ */
