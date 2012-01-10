#ifndef THREADS_H_
#define THREADS_H_

#include "SCTaskPool.h"

typedef struct{
	int id;
	int numOfProducedTasks;
	double producerThroughput;
	AtomicStatistics atomicStats;
} producerStats;

typedef struct{
	int id;
	int numOfRetrievedTasks;
	double consumerThroughput;
	AtomicStatistics atomicStats;
} consumerStats;

typedef struct producerArg{
	int id;
} producerArg;

typedef struct{
	int id;
	SCTaskPool** poolPtr;
} consumerArg;

class syncFlags{	
	static bool simulationStart;
	static int allocatedPoolsCounter;
	static bool simulationStop;
public:
	static void start(){simulationStart = true;}
	static void incPoolsCounter(){__sync_fetch_and_add(&allocatedPoolsCounter,1);}
	static void stop(){simulationStop = true;}
	static int getAllocatedPoolsCounter(){return allocatedPoolsCounter;}
	static bool getStartFlag(){return simulationStart;}
	static bool getStopFlag(){return simulationStop;}
};

	void assignToCPU(int cpu);
	void* prodRun(void* arg);
	void* consRun(void* arg);


#endif //THREADS_H_