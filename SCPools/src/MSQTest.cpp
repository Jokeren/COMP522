#include "MSQueue.hpp"
#include "Task.h"
#include <stdlib.h>
#include <iostream>
using namespace std;

static volatile bool stop = false;
static volatile bool start = false;

void* consRunTest(void* _arg) {
	MSQueue<DummyTask*>* queue = (MSQueue<DummyTask*>*)_arg;
	DummyTask* t;
	AtomicStatistics* stat = new AtomicStatistics();

	while(!start) {}

	while(!stop) {
		queue->dequeue(t, stat);
	}

	return NULL;
}

void* prodRunTest(void* _arg) {
	MSQueue<DummyTask*>* queue = (MSQueue<DummyTask*>*)_arg;
	DummyTask* t = new DummyTask();
	AtomicStatistics* stat = new AtomicStatistics();

	while(!start) {}

	while(!stop) {
		queue->enqueue(t, stat);
	}

	return NULL;
}

int main1(int argc, char* argv[])
{
	int consNum = 16;
	int prodNum = 16;


	MSQueue<DummyTask*>* queue = new MSQueue<DummyTask*>();

	//create and start threads
	pthread_t* cThreads = new pthread_t[consNum];
	pthread_t* pThreads = new pthread_t[prodNum];

	for(int i = 0; i < consNum; i++)
	{
		pthread_create(&cThreads[i],NULL, consRunTest,(void*)queue);
	}
	for(int i = 0; i < prodNum; i++)
	{
		pthread_create(&pThreads[i],NULL, prodRunTest,(void*)queue);
	}

	// start
	cout << "Starting the run..." << endl;
	start = true;
	usleep(10000000); // sleep for 10 seconds
	cout << "Stopping the threads..." << endl;
	stop = true;

	// wait for child threads
	for(int i = 0; i < prodNum; i++)
	{
		pthread_join(pThreads[i], NULL);
	}
	for(int i = 0; i < consNum; i++)
	{
		pthread_join(cThreads[i], NULL);
	}

	cout << "That's all" << endl;
}
