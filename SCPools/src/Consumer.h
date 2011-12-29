/*
 * Consumer.h
 *
 *  Created on: Dec 28, 2011
 */

#ifndef CONSUMER_H_
#define CONSUMER_H_

#include "SCTaskPool.h"
#include "commons.h"

class Consumer {
public:
	Consumer(int id);
	virtual ~Consumer();

	virtual OpResult consume(Task*& t);

protected:
	virtual OpResult consumeEmpty(Task*& t);

protected:
	int consumersNum;
	int id;
	SCTaskPool** consumers;
	SCTaskPool* myPool;
	int stealIterations;
};

#endif /* CONSUMER_H_ */
