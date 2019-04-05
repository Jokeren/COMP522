/*
 * Consumer.cpp
 *
 *  Created on: Dec 28, 2011
 */

#include "Consumer.h"
#include "Configuration.h"
#include "ArchEnvironment.h"
#include <assert.h>

Consumer::Consumer(int _id)
	: id(_id)
{
	consumers = ArchEnvironment::getInstance()->getSortedConsumers(*this);
	myPool = consumers[0];

	assert(Configuration::getInstance()->getVal(this->consumersNum, "consumersNum"));
	if (Configuration::getInstance()->getVal(stealIterations, "stealIterations") == false) {
		stealIterations = 2; // default
	}
	stat = new AtomicStatistics();
	myPool->setAtomicStatistics(stat);
	stealingCounter = 0;
	curStealPool = NULL;
}

Consumer::~Consumer() {
	delete stat;
}

OpResult Consumer::consume(Task*& t) {
	// The pool returns EMPTY only if it didn't succeed to find a task
	// after the predefined amount of attempts (part of pool's implementation).
	// In this case a consumer traverses over other pools until finds the pool with
	// the stealing score higher than a given threshold.
	// The threshold is repeatedly decreased to zero.

	OpResult res = myPool->consume(t, stat);
	if (res == SUCCESS || stealIterations <= 0) return res;

	for(int stealIter = 0; stealIter < stealIterations; stealIter++) {
		if (myPool->consume(t,stat) == SUCCESS) return SUCCESS;
	}

	float stealThreshold = myPool->getStealingThreshold();
	if (curStealPool != NULL && curStealPool->getStealingScore() >= stealThreshold) {
		stealingCounter++;
		t = myPool->steal(curStealPool, stat);
		if (t != NULL) return SUCCESS;
	}

	for(int c = 1; c < consumersNum; c++) {
		if (consumers[c]->getStealingScore() >= stealThreshold) {
			// try to steal from that pool
			stealingCounter++;
			t = myPool->steal(consumers[c],stat);
			if (t != NULL) {
				// successful stealing
				curStealPool = consumers[c];
				return SUCCESS;
			}
		}
	}

	// the container is probably empty
	return consumeEmpty(t);
}

OpResult Consumer::consumeEmpty(Task*& t) {
	return EMPTY; // TODO: still a stub
}
