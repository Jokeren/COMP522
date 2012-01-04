#include "MSQTaskPool.h"

MSQTaskPool::MSQTaskPool():stealingThreshold(STEALING_THRESHOLD){
	Q = new MSQueue<Task*>();
	prodContext = new MSQProducerContext(this);
}

MSQTaskPool::~MSQTaskPool(){
	Task* t;
	while(Q->dequeue(t))
	{
		delete t;
	}
	delete Q;
}

SCTaskPool::ProducerContext* MSQTaskPool::getProducerContext(const Producer& prod){return prodContext;}

OpResult MSQTaskPool::consume(Task*& t){
	if(Q->dequeue(t))
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

Task* MSQTaskPool::steal(SCTaskPool* from){
	Task* task = NULL;
	from->consume(task);	
	return task;
}

// a dummy function for the moment
int MSQTaskPool::getEmptynessCounter() const{
	return 0;
}

void MSQTaskPool::insert(const Task& task){
	const Task* t = &task;
	Q->enqueue(const_cast <Task*> (t));
}

OpResult MSQProducerContext::produce(const Task& task, bool& changeConsumer){
	changeConsumer = false;
	pool->insert(task);
	return SUCCESS;
}

void MSQProducerContext::produceForce(const Task& task){
	pool->insert(task);
}


