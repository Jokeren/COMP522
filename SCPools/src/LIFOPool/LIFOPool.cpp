#include "SCTaskPool.h"
#include "LIFOPool.h"
#include "lifo.h"
#include "Atomic.h"

LIFOPool::LIFOPool() :
		prodCtx(new LIFOProducerContext(this)),
		stack(new Lifo()),
		size(0)
{
}

void LIFOPool::insert(Task* t) {
	Lifo::Node* n = new Lifo::Node();
	n->task = t;
	stack->push(n);
	FAA(&size, 1);
}

LIFOPool::~LIFOPool() {
	delete stack;
	delete prodCtx;
};

LIFOPool::ProducerContext* LIFOPool::getProducerContext(const Producer& prod) {
	return prodCtx;
}

OpResult LIFOPool::consume(Task*& t, AtomicStatistics* stat) {
	Lifo::Node* n = stack->pop();
	if (n == NULL) {
		t = NULL; return EMPTY;
	}
	t = n->task;
	FAS(&size, 1);
	return SUCCESS;
}

float LIFOPool::getStealingScore() const {
	return size;
}

float LIFOPool::getStealingThreshold() const {
	return 10;
}

Task* LIFOPool::steal(SCTaskPool* from, AtomicStatistics* stat) {
	Task* res = NULL;
	if (from->consume(res, stat) == SUCCESS) return res;
	return NULL;
}

int LIFOPool::getEmptynessCounter() const {
	return 0;
}

LIFOProducerContext::LIFOProducerContext(LIFOPool* p) : pool(p) {}

OpResult LIFOProducerContext::produce(Task& t, bool& changeConsumer, AtomicStatistics* stat) {
	pool->insert(&t);
	return SUCCESS;
}
void LIFOProducerContext::produceForce(Task& t, AtomicStatistics* stat) {
	pool->insert(&t);
};

