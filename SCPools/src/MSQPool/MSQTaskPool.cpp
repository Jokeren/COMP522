#include "MSQTaskPool.h"

MSQTaskPool::MSQTaskPool():stealingThreshold(STEALING_THRESHOLD){
	Q = new MSQueue<Task*>();
	prodContext = new MSQProducerContext(this);
}

MSQTaskPool::~MSQTaskPool(){
	AtomicStatistics* dummyStat = new AtomicStatistics();
	Task* t;
	while(Q->dequeue(t, dummyStat))
	{
		delete t;
	}
	delete dummyStat;
	delete Q;
}

SCTaskPool::ProducerContext* MSQTaskPool::getProducerContext(const Producer& prod){return prodContext;}

OpResult MSQTaskPool::consume(Task*& t, AtomicStatistics* stat){
	if(Q->dequeue(t, stat))
	{
		return SUCCESS;
	}
	return EMPTY;
}

float MSQTaskPool::getStealingThreshold() const{
	return stealingThreshold;
}

float MSQTaskPool::getStealingScore() const{
	return Q->getSize();
}

Task* MSQTaskPool::steal(SCTaskPool* from, AtomicStatistics* stat){
	Task* task = NULL;
	from->consume(task, stat);	
	return task;
}

// a dummy function for the moment
int MSQTaskPool::getEmptynessCounter() const{
	return 0;
}

void MSQTaskPool::insert(Task& task, AtomicStatistics* stat){
	Task* t = &task;
	Q->enqueue(t, stat);
}

OpResult MSQProducerContext::produce(Task& task, bool& changeConsumer, AtomicStatistics* stat){
	changeConsumer = false;
	pool->insert(task, stat);
	return SUCCESS;
}

void MSQProducerContext::produceForce(Task& task, AtomicStatistics* stat){
	pool->insert(task, stat);
}


