#include "Threads.h"
#include "MSQTaskPool.h"
#include "NoFIFOPool.h"
#include "NoFIFOCASPool.h"
#include "hp/hp.h"
#include <pthread.h>
#include <sched.h>
#include <assert.h>
//#include <numa.h> 	// for debug only
#include <iostream>
#include <list>
#include <time.h>
#include "ArchEnvironment.h"
using namespace std;

volatile bool syncFlags::simulationStart = false;
volatile int syncFlags::allocatedPoolsCounter = false;
volatile bool syncFlags::simulationStop = false;

void assignToCPU(int cpu){

	pthread_t threadId = pthread_self();
	cpu_set_t cpuSet;
	CPU_ZERO(&cpuSet);
	CPU_SET(cpu,&cpuSet);
	int s = pthread_setaffinity_np(threadId,sizeof(cpu_set_t),&cpuSet);
	//int s = sched_setaffinity(0,sizeof(cpu_set_t),&cpuSet);
	if(s != 0)
	{
		cout << "Thread Assignment Failed! exiting.." << endl;
		exit(-1);
	}
}

void* prodRun(void* _arg){
	producerArg* arg = (producerArg*)_arg;
	HP::threadRegister(HP::initHPData(4,arg->numOfThreads));
	int id = arg->id;
	
	// handle thread's affinity
	string forceAssignment;
	Configuration::getInstance()->getVal(forceAssignment, "forceAssignment");
	if(forceAssignment.compare("yes") == 0)
	{
		int cpu = ArchEnvironment::getInstance()->getProducerCore(id);
		assignToCPU(cpu);
	}
	/***** debug ****/
	//cout << "p " << sched_getcpu() << endl; 
	//int _cpu = sched_getcpu();
	//cout << "p" << id << " " << _cpu << " " << numa_node_of_cpu(_cpu) << endl;
	/***************/
	// wait until simulation starts
	while(!syncFlags::getStartFlag()){}
	
	// create producer and run
	int peakLength, timeBetweenPeaks;
	assert(Configuration::getInstance()->getVal(peakLength, "peakLength"));
	assert(Configuration::getInstance()->getVal(timeBetweenPeaks, "timeBetweenPeaks"));
	list<long>* timeMeasurements = new list<long>();
	struct timespec* ts = new timespec();
	Producer* producer = new Producer(id);
	DummyTask* task = new DummyTask();
	// peak-silence loop
	while(!syncFlags::getStopFlag())
	{
		while(arg->pause == true) {} // wait if you're paused
		// peak period
		/* sample peak start time */
		clock_gettime(_POSIX_MONOTONIC_CLOCK,ts);
		unsigned long start_ns = ts->tv_nsec;
		unsigned long start_sec = ts->tv_sec;
		/* task generation and insertion */
		for(int i = 0; i < peakLength; i++)
		{
			// if i%100 == 0 - set task insertion time to current time in ms
			if(i%100 == 0)
			{
				clock_gettime(_POSIX_MONOTONIC_CLOCK,ts);
				task->insertionTime_sec = ts->tv_sec;
				task->insertionTime_ns = ts->tv_nsec;
			}
			producer->produce(*task);
		}
		/* sample peak end time and update timeMeasurements */
		clock_gettime(_POSIX_MONOTONIC_CLOCK,ts);
		unsigned long finish_ns = ts->tv_nsec;
		unsigned long finish_sec = ts->tv_sec;
		if(start_sec < finish_sec)
		{
			finish_ns += 1000000000;
		}
		timeMeasurements->push_front(finish_ns-start_ns);
		// silent period
		if(timeBetweenPeaks > 0){1000*usleep(timeBetweenPeaks);}
	}
	delete ts;
	
	
	// build the returned stats
	producerStats* prodStats = new producerStats();
	prodStats->id = id;
	prodStats->numOfProducedTasks = peakLength*timeMeasurements->size();
	list<long>::iterator it;
	double sum = 0;
	for(it = timeMeasurements->begin(); it !=  timeMeasurements->end(); it++)
	{
		sum += ((double)*it)/1000000;
	}
	double averageInsertionTime = sum/timeMeasurements->size(); // average burst length in [ms]
	prodStats->producerThroughput = peakLength *(1/averageInsertionTime);  //insertion throughput in tasks/ms
	prodStats->atomicStats = *(producer->getStat());
	delete producer;
	delete timeMeasurements;
	return (void*)prodStats;
}


void* consRun(void* _arg){
	consumerArg* arg = (consumerArg*)_arg;
	HP::threadRegister(HP::initHPData(4,arg->numOfThreads));
	int id = arg->id;
	
	// handle thread's affinity
	string forceAssignment;
	Configuration::getInstance()->getVal(forceAssignment, "forceAssignment");
	if(forceAssignment.compare("yes") == 0)
	{
		int cpu = ArchEnvironment::getInstance()->getConsumerCore(id);
		assignToCPU(cpu);
	}
	/***** debug ********/
	//cout << "c " << sched_getcpu() << endl;
	//int _cpu = sched_getcpu();
	//cout << "c" << id << " " << _cpu << " " << numa_node_of_cpu(_cpu) << endl;
	/*******************/
	// allocate consumer's thread pool according to pool's type
	string poolType;
	assert(Configuration::getInstance()->getVal(poolType, "poolType"));
	if(poolType.compare("MSQTaskPool") == 0)
	{
		*(arg->poolPtr) = new MSQTaskPool();
	}
	else if (poolType.compare("NoFIFOPool") == 0)
	{
		*(arg->poolPtr) = new NoFIFOPool(arg->numOfProducers, arg->id);
	}
	else if (poolType.compare("NoFIFOCASPool") == 0)
	{
		*(arg->poolPtr) = new NoFIFOCASPool(arg->numOfProducers, arg->id);
	}
	else
	{
		cout << "ERROR: pool type not supported. exiting.." << endl;
		exit(1);
	}
	syncFlags::incPoolsCounter();
	
	// wait until simulation starts
	while(!syncFlags::getStartFlag()){}
	
	// create consumer and run
	int numOfTasks = 0;
	double throughput = 0;
	struct timespec* ts = new timespec();
	Consumer* consumer = new Consumer(id);
	// retrieve tasks in a loop
	/* sample loop start time */
	clock_gettime(_POSIX_MONOTONIC_CLOCK,ts);
	unsigned long start_sec = ts->tv_sec;
	unsigned long start_ns = ts->tv_nsec;
	/* retrieval loop */
	while(!syncFlags::getStopFlag())
	{
		while(arg->pause == true) {} // wait if you're paused
		Task* task;
		if(consumer->consume(task) != SUCCESS)
		{
			continue;
		}
		numOfTasks++;
		//DummyTask* dt = (DummyTask*)task;
		//dt->run(NULL);  // run task
		//delete dt;  // delete task
	}
	/* sample loop end time */
	clock_gettime(_POSIX_MONOTONIC_CLOCK,ts);
	unsigned long finish_sec = ts->tv_sec;
	unsigned long finish_ns = ts->tv_nsec;
	/* update throughput */
	if(finish_sec > start_sec)  
	{
		finish_ns += 1000000000;
		finish_sec--;
	}
	double loopTime = 1000*(finish_sec - start_sec) + ((double)(finish_ns-start_ns))/1000000;
	throughput = ((double)numOfTasks)/loopTime;
	delete ts;
		
	// build the returned stats
	consumerStats* consStats = new consumerStats();
	consStats->id = id;
	consStats->numOfRetrievedTasks = numOfTasks;
	consStats->consumerThroughput = throughput;
	consStats->atomicStats = *(consumer->getStat());
	consStats->stealingCounter = consumer->getStealingCounter();
	delete consumer;
	return (void*)consStats;
}
