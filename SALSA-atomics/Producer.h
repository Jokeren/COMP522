/*
 * Producer.h
 *
 *  Created on: Dec 28, 2011
 */

#ifndef PRODUCER_H_
#define PRODUCER_H_

#include "SCTaskPool.h"

class Producer {
public:
	Producer(int id, int curConsumerIdx = 0);
	virtual ~Producer();

	virtual void produce(Task& t);

	int getId() const{ return id;}
	AtomicStatistics* getStat(){return stat;}
	
private:
	int id;
	int consumersNum;
	SCTaskPool::ProducerContext* curProdContext;
	int curConsumerIdx;
	bool producerMigrate;
	SCTaskPool** consumers;
	AtomicStatistics* stat;
};

#endif /* PRODUCER_H_ */
