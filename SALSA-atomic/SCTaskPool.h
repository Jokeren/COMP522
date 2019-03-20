#ifndef SCTASKPOOL_H_
#define SCTASKPOOL_H_

#include "Task.h"
#include "commons.h"
#include "AtomicStatistics.h"

class Producer;

// Single consumer task pool with stealing
class SCTaskPool {
public:
	// the code of producer
	class ProducerContext {
	public:
		virtual ~ProducerContext() {};

		virtual OpResult produce(Task& t, bool& changeConsumer, AtomicStatistics* stat) = 0;
		virtual void produceForce(Task& t, AtomicStatistics* stat) = 0;
	};

	/* constructor to be used when derived TaskPool is aware to the number of producers.
	   Needs to be explicitly called by constructor of derived class. */
	SCTaskPool(int _numProducers) : numProducers(_numProducers) {};
	
	/* default constructor - to be used when derived TaskPool is unaware to the number of 
	producers. Called automatically by constructor of derived class */
	SCTaskPool():numProducers(0){};
	
	virtual ~SCTaskPool() {};

	virtual ProducerContext* getProducerContext(const Producer& prod) = 0;
	virtual OpResult consume(Task*& t, AtomicStatistics* stat) = 0;

	virtual float getStealingScore() const = 0;
	virtual float getStealingThreshold() const = 0;
	virtual Task* steal(SCTaskPool* from, AtomicStatistics* stat) = 0;

	virtual int getEmptynessCounter() const = 0;

	virtual void setAtomicStatistics(AtomicStatistics* stat) {};

protected:
	int numProducers;
};


#endif /* SCTASKPOOL_H_ */
