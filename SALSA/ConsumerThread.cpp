
#include "ConsumerThread.h"
#include <assert.h>

bool ConsumerThread::stopFlag = false;

ConsumerThread::ConsumerThread(int id) {
	this->numOfTasks = 0;
	this->throughput = 0;
	this->ts = new timespec();
	this->id = id;
	this->consumer = new Consumer(id);
}


/* consumer run: retrieve tasks in a loop */
void ConsumerThread::run(){
	/* get retrieval loop start time */
	clock_gettime(_POSIX_MONOTONIC_CLOCK,ts);
	unsigned long start_sec = ts->tv_sec;
	unsigned long start_ns = ts->tv_nsec;
	
	/* retrieval loop */
	while(!stopFlag)
	{
		Task* task;
		if(consumer->consume(task) != SUCCESS)
		{
			continue;
		}
		DummyTask* dt = (DummyTask*)task;
		numOfTasks++;
		dt->run(NULL);  // run task
		delete dt;  // delete task		
	}
	
	/* get retrieval loop end time */
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
}


ConsumerThread::~ConsumerThread(){
	delete consumer;
}

