/*
 * Producer.cpp
 *
 *  Created on: Dec 28, 2011
 */

#include "Producer.h"
#include "Configuration.h"
#include "ArchEnvironment.h"
#include "Task.h"
#include <assert.h>

#include <iostream>
using namespace std;

Producer::Producer(int _id)
 : id(_id)
{
	this->consumers = ArchEnvironment::getInstance()->getSortedConsumers(*this);
	this->curConsumerIdx = 0;
	this->curProdContext = consumers[0]->getProducerContext(*this);
	assert(Configuration::getInstance()->getVal(this->consumersNum, "consumersNum"));
	this->stat = new AtomicStatistics();
}

Producer::~Producer() {
	delete stat;
}

void Producer::produce(Task& t) {
//	curProdContext->produceForce(t, stat);

	bool changeConsumer = false;
	OpResult res = curProdContext->produce(t, changeConsumer, stat);

	if (res == SUCCESS) {
		if (curConsumerIdx != 0 && changeConsumer) {
			curConsumerIdx = 0;
			curProdContext = consumers[0]->getProducerContext(*this);
		}
		return;
	}

	// try to insert the task into another consumer pool
	for(int i = 0; i < consumersNum; i++) {
		curConsumerIdx = i;
		curProdContext = consumers[i]->getProducerContext(*this);

		if (curProdContext->produce(t, changeConsumer, stat) == SUCCESS) return;
	}

	// nothing helped, want to insert it forcefully to the closest consumer
	curConsumerIdx = 0;
	curProdContext = consumers[0]->getProducerContext(*this);
	curProdContext->produceForce(t, stat);
}

