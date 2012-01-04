#ifndef PRODUCERTHREAD_H_
#define PRODUCERTHREAD_H_
#include "Producer.h"
#include "Configuration.h"
#include <list>
#include <time.h>
using namespace std;

class ProducerThread {
	int peakLength;  //number of tasks that need to be produced in each burst
	int timeBetweenPeaks;  // idle time between bursts in ms
	static bool startFlag;
	static bool stopFlag;	// a flag indicating that all producer threads should stop
	list<long>* timeMeasurements;  // a list of the actual durations of the bursts
	struct timespec* ts;	// a struct used for time sampling
	int taskId;  // index of the last task produced
	
	int id;
	Producer* producer;

public:
	ProducerThread(int id);
	void run();
	DummyTask* createTask(int id){return new DummyTask(id);}
	static void startSimulation(){startFlag = true;}
	static void stop(){stopFlag = true;}
	virtual ~ProducerThread();
	int getId(){return id;}

	// getters
	int getPeakLength(){return peakLength;}
	list<long>* getTimeMeasurements(){return timeMeasurements;}
};

#endif /* PRODUCER_H_ */
