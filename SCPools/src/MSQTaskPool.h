#ifndef MSQTASKPOOL_H_
#define MSQTASKPOOL_H_

#include "MSQueue.hpp"
#include "SCTaskPool.h"
#define STEALING_THRESHOLD 100

class MSQProducerContext;

class MSQTaskPool : public SCTaskPool{
	MSQueue<Task*>* Q;
	MSQProducerContext* prodContext;
	const float stealingThreshold ;
	void insert(const Task& task);
public:
	MSQTaskPool();
	virtual ~MSQTaskPool();
	
	ProducerContext* getProducerContext(const Producer& prod);
	OpResult consume(Task*& t);

	float getStealingScore() const;
	float getStealingThreshold() const;
	Task* steal(SCTaskPool* from);

	int getEmptynessCounter() const;
	
	friend class MSQProducerContext;
};

class MSQProducerContext : public SCTaskPool::ProducerContext{
	MSQTaskPool* pool;
	public:
	MSQProducerContext(MSQTaskPool* _pool):pool(_pool){}
	virtual ~MSQProducerContext(){};
	OpResult produce(const Task& task, bool& changeConsumer);
	void produceForce(const Task& task);
};

#endif  //MSQTASKPOOL_H_