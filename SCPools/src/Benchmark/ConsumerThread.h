#ifndef CONSUMERTHREAD_H_
#define CONSUMERTHREAD_H_

#include "Consumer.h"
#include "Configuration.h"
#include <time.h>
#include <list>
#include <iostream>
#include <stdlib.h>
using namespace std;

class ConsumerThread {
	static bool stopFlag;  // a flag indicating all consumers should stop
	int numOfTasks;  //number of retrieved  tasks
	double throughput;  // tasks retrieved per ms
	struct timespec* ts;  //for time measurements
	int id;
	Consumer* consumer;
	
public:
	ConsumerThread(int id);
	static void stop(){stopFlag = true;}
	void run();
	virtual ~ConsumerThread();

	// getters
	double getThroughput(){return throughput;}
	int getNumOfTasks(){return numOfTasks;}
	int getId(){return id;}
};
#endif // CONSUMERTHREAD_H_
