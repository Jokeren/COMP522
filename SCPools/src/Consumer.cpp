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
	stealingCounter = 0;
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
	if(stealIterations <= 0)
	{
		return myPool->consume(t,stat);
	}
	float stealThreshold = myPool->getStealingThreshold();
	float stealThresholdDelta = stealThreshold / stealIterations;

	for(int stealIter = 0; stealIter < stealIterations; stealIter++) {
		if (myPool->consume(t,stat) == SUCCESS) return SUCCESS;

		for(int c = 1; c < consumersNum; c++) {
			if (consumers[c]->getStealingScore() >= stealThreshold) {
				// try to steal from that pool
				stealingCounter++;
				Task* stolenTask = myPool->steal(consumers[c],stat);
				if (stolenTask != NULL) {
					// successful stealing
					t = stolenTask;
					return SUCCESS;
				}
			}
		}
		stealThreshold -= stealThresholdDelta;
	}

	// the container is probably empty
	return consumeEmpty(t);
}

OpResult Consumer::consumeEmpty(Task*& t) {
	return EMPTY; // TODO: still a stub
}
