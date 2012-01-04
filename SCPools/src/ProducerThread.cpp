
#include "ProducerThread.h"
#include <assert.h>

bool ProducerThread::stopFlag = false;
bool ProducerThread::startFlag = false;


ProducerThread::ProducerThread(int id) {
	this->id = id;
	this->producer = new Producer(id);
	assert(Configuration::getInstance()->getVal(peakLength, "peakLength"));
	assert(Configuration::getInstance()->getVal(timeBetweenPeaks, "timeBetweenPeaks"));
	this->timeMeasurements = new list<long>();
	this->taskId = 0;
	ts = new timespec();
}


ProducerThread::~ProducerThread(){
	delete timeMeasurements;
	delete producer;
}

/*
	producer run:
	- while not stopped - do iterations of the followings:
		peak period: generate and insert to container <peakLength> tasks
		silent period: sleep for a random time
	note that stopping the producer is only possible between iterations.
	This means that once a producer started a peak period, it will always complete it before finishing
*/
void ProducerThread::run(){
	// busy-wait until simulation starts
	while(!startFlag){}
	
	// peak-silence loop
	while(!stopFlag)
	{
		// peak period
		/* sample peak start time */
		clock_gettime(_POSIX_MONOTONIC_CLOCK,ts);
		unsigned long start_ns = ts->tv_nsec;
		unsigned long start_sec = ts->tv_sec;
		
		/* task generation and insertion */
		for(int i = 0; i < peakLength; i++)
		{
			DummyTask* task = createTask(taskId++);
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
		/* on even iterations - draw a pair of values from a normal distribution, to be used as sleep times */
		if(timeBetweenPeaks > 0){1000*usleep(timeBetweenPeaks);}
	}
	delete ts;
}


