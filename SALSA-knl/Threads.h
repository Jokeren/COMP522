#ifndef THREADS_H_
#define THREADS_H_

#include "SCTaskPool.h"

typedef struct{
	int id;
	unsigned long numOfProducedTasks;
	double producerThroughput;
	AtomicStatistics atomicStats;
} producerStats;

typedef struct{
	int id;
	unsigned long numOfRetrievedTasks;
	double consumerThroughput;
	AtomicStatistics atomicStats;
	unsigned long stealingCounter;
} consumerStats;

typedef struct producerArg{
	int numOfThreads;
	int id;
	int consumerId;
	volatile bool pause;
} producerArg;

typedef struct{
	int id;
	int numOfThreads;
	int numOfProducers;
	volatile bool pause;
	SCTaskPool** poolPtr;
} consumerArg;

class syncFlags{	
	static volatile bool simulationStart;
	static volatile int allocatedPoolsCounter;
	static volatile bool simulationStop;
public:
	static void start(){simulationStart = true;}
	static void incPoolsCounter(){__sync_fetch_and_add(&allocatedPoolsCounter,1);}
	static void stop(){simulationStop = true;}
	static int getAllocatedPoolsCounter(){return allocatedPoolsCounter;}
	static bool getStartFlag(){return simulationStart;}
	static bool getStopFlag(){return simulationStop;}
	static void assignToCPU(int cpu);

};

	void assignToCPU(int cpu);
	void* prodRun(void* arg);
	void* consRun(void* arg);


#endif //THREADS_H_
