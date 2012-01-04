#ifndef CONSUMERTHREAD_H_
#define CONSUMERTHREAD_H_

#include "Consumer.h"
#include "Configuration.h"
#include "MSQTaskPool.h"
#include <time.h>
#include <list>
#include <iostream>
#include <stdlib.h>
using namespace std;

class ConsumerThread {
	static bool startFlag;
	static bool stopFlag;  // a flag indicating all consumers should stop
	static int allocatedPoolsCounter;
	int numOfTasks;  //number of retrieved  tasks
	double throughput;  // tasks retrieved per ms
	struct timespec* ts;  //for time measurements
	int id;
	Consumer* consumer;
	bool retrieveTask(); // returns true is retrieval was successful and false otherwise
	
public:
	ConsumerThread(int id, SCTaskPool** poolPtr);
	static void startSimulation(){startFlag = true;}
	static void stop(){stopFlag = true;}
	void run();
	virtual ~ConsumerThread();

	// getters
	double getThroughput(){return throughput;}
	int getNumOfTasks(){return numOfTasks;}
	int getId(){return id;}
	static int getAllocatedPoolsCounter(){return allocatedPoolsCounter;}
	
};
#endif // CONSUMERTHREAD_H_