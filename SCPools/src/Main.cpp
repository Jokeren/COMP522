/*
	Main test program
*/
#include "Threads.h"
#include "ArchEnvironment.h"
#include <assert.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		cout << "no configuration file found. exiting.." << endl;
		exit(1);
	}
	setenv("WS_CONFIG", argv[1], 1);
	int prodNum, consNum;
	assert(Configuration::getInstance()->getVal(prodNum, "producersNum"));
	assert(Configuration::getInstance()->getVal(consNum, "consumersNum"));
	
	ArchEnvironment::getInstance()->threadToCoreChipMapping();
	
	// craete new pool pointers array - one for each consumer. Actual pools will be allocated by the consumers.
	SCTaskPool** pools = new SCTaskPool*[consNum];
	for(int i = 0; i < consNum; i++)
	{
		pools[i] = NULL;
	}

	consumerArg* consArgs = new consumerArg[consNum];
	// value of i will serve as an id for the corresponding consumer from now on
	for(int i = 0; i < consNum; i++)
	{
		consArgs[i].id = i;
		consArgs[i].poolPtr = &pools[i];
	}
	
	producerArg* prodArgs = new producerArg[prodNum];
	// value of i will serve as an id for the corresponding producer from now on
	for(int i = 0; i < prodNum; i++)
	{
		prodArgs[i].id = i;
	}
	
	//create and start threads
	pthread_t* cThreads = new pthread_t[consNum];
	pthread_t* pThreads = new pthread_t[prodNum];
	for(int i = 0; i < consNum; i++)
	{
		pthread_create(&cThreads[i],NULL, consRun,(void*)&consArgs[i]);
	}
	for(int i = 0; i < prodNum; i++)
	{
		pthread_create(&pThreads[i],NULL, prodRun,(void*)&prodArgs[i]);
	}
	
	// busy-wait until all pools have been allocated
	while(syncFlags::getAllocatedPoolsCounter() < consNum){}
	// set consumer-to-pool mapping in ArchEnvironment
	ArchEnvironment::getInstance()->setConsumerToPoolMapping(pools);
	
	
	void** prodStatsArray = new void*[prodNum];
	void** consStatsArray = new void*[consNum];
	
	// let producers and consumers do their thing...
	int timeToRun;
	assert(Configuration::getInstance()->getVal(timeToRun, "timeToRun"));
	syncFlags::start();	
	usleep(1000*timeToRun);
	// consider using non-static stop flag (flag for each thread)
	syncFlags::stop();
	syncFlags::stop();
	
	
	// wait for child threads
	for(int i = 0; i < prodNum; i++)
	{
		pthread_join(pThreads[i], &prodStatsArray[i]);
	}
	for(int i = 0; i < consNum; i++)
	{
		pthread_join(cThreads[i],&consStatsArray[i]);
	}
	
	// sum and print the statistics
	int TotalNumOfProducedTasks = 0;
	double TotalInsertionThroughput = 0;
	int TotalNumOfRetrievedTasks = 0;
	double TotalSystemThroughput = 0;
	for(int i = 0; i < prodNum; i++)
	{
		producerStats* stats = (producerStats*)prodStatsArray[i];
		TotalNumOfProducedTasks += stats->numOfProducedTasks;
		TotalInsertionThroughput += stats->producerThroughput;
		delete stats;		
	}
	for(int i = 0; i < consNum; i++)
	{
		consumerStats* stats = (consumerStats*)consStatsArray[i];
		TotalNumOfRetrievedTasks += stats->numOfRetrievedTasks;
		TotalSystemThroughput += stats->consumerThroughput;
		delete stats;
	}
	cout << "Total number of inserted tasks = " << TotalNumOfProducedTasks << endl;
	cout << "Peak Insertion throughput = " << TotalInsertionThroughput << endl;
	cout << "Total Number of retrieved tasks = " << TotalNumOfRetrievedTasks << endl;
	cout << "System Throughput = " << TotalSystemThroughput << endl;
	
	// free allocated memory
	for(int i = 0; i < consNum; i++)
	{
		delete pools[i];
	}
	delete[] pools;
	delete[] consArgs;
	delete[] prodArgs;
	delete[] cThreads;
	delete[] pThreads;
	delete[] prodStatsArray;
	delete[] consStatsArray;
	return 0;
}




