#ifndef MSQTASKPOOL_H_
#define MSQTASKPOOL_H_

#include "MSQueue.hpp"
#include "SCTaskPool.h"
#define STEALING_THRESHOLD 10
#include "AtomicStatistics.h"
class MSQProducerContext;

class MSQTaskPool : public SCTaskPool{
	MSQueue<Task*>* Q;
	MSQProducerContext* prodContext;
	const float stealingThreshold ;
	void insert(Task& task, AtomicStatistics* stat);
public:
	MSQTaskPool();
	virtual ~MSQTaskPool();
	
	ProducerContext* getProducerContext(const Producer& prod);
	OpResult consume(Task*& t, AtomicStatistics* stat);

	float getStealingScore() const;
	float getStealingThreshold() const;
	Task* steal(SCTaskPool* from, AtomicStatistics* stat);

	int getEmptynessCounter() const;
	
	friend class MSQProducerContext;
};

class MSQProducerContext : public SCTaskPool::ProducerContext{
	MSQTaskPool* pool;
	public:
	MSQProducerContext(MSQTaskPool* _pool):pool(_pool){}
	virtual ~MSQProducerContext(){};
	OpResult produce(Task& task, bool& changeConsumer, AtomicStatistics* stat);
	void produceForce(Task& task, AtomicStatistics* stat);
};

#endif  //MSQTASKPOOL_H_
