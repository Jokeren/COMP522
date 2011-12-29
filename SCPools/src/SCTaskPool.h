#ifndef SCTASKPOOL_H_
#define SCTASKPOOL_H_

#include "Task.h"
#include "commons.h"

class Producer;

class SCTaskPool {
public:
	class ProducerContext {
	public:
		virtual ~ProducerContext() {};

		virtual OpResult produce(const Task& t, bool& changeConsumer) = 0;
		virtual void produceForce(const Task& t) = 0;
	};

	SCTaskPool(int _numProducers) : numProducers(_numProducers) {};
	virtual ~SCTaskPool() {};

	virtual ProducerContext* getProducerContext(const Producer& prod) = 0;
	virtual OpResult consume(Task*& t) = 0;

	virtual float getStealingScore() const = 0;
	virtual float getStealingThreshold() const = 0;
	virtual Task* steal(SCTaskPool& from) = 0;

	virtual int getEmptynessCounter() const = 0;

protected:
	int numProducers;
};


#endif /* SCTASKPOOL_H_ */
