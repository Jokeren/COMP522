/*
	Main test program
*/
#include "Threads.h"
#include "ArchEnvironment.h"
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>

using namespace std;

void run(int consNum, consumerArg* consArgs, int prodNum, producerArg* prodArgs) {
	// let producers and consumers do their thing...
	int timeToRun;
	assert(Configuration::getInstance()->getVal(timeToRun, "timeToRun"));
	cout << "Time to run = " << timeToRun << " msec" << endl;

	bool prodFluctuations = false;
	Configuration::getInstance()->getVal(prodFluctuations, "prodFluctuations");
	cout << "Producer fluctuations are turned " << (prodFluctuations ? "on" : "off") << endl;

	bool consFluctuations = false;
	Configuration::getInstance()->getVal(consFluctuations, "consFluctuations");
	cout << "Consumer fluctuations are turned " << (consFluctuations ? "on" : "off") << endl;

	int pausedThreads=1;
	Configuration::getInstance()->getVal(pausedThreads, "pausedThreads");
	if (prodFluctuations || consFluctuations) {
		cout << "Threads to pause = " << pausedThreads << endl;
	}


	syncFlags::start();
	cout << "Starting the run..." << endl;
	int* pausedProducers = new int[pausedThreads];
	int* pausedConsumers = new int[pausedThreads];
	for(int i = 0; i < 10; i++) {
		if (prodFluctuations) {
			for(int p = 0; p < pausedThreads; p++) {
				pausedProducers[p] = (rand()%prodNum);
				prodArgs[pausedProducers[p]].pause = true;
			}
		}
		if (consFluctuations) {
			for(int c = 0; c < pausedThreads; c++) {
				pausedConsumers[c] = (rand()%consNum);
				consArgs[pausedConsumers[c]].pause = true;
			}
		}

		usleep(100*timeToRun);

		if (prodFluctuations) {
			for(int p = 0; p < pausedThreads; p++) {
				prodArgs[pausedProducers[p]].pause = false;
			}
		}
		if (consFluctuations) {
			for(int c = 0; c < pausedThreads; c++) {
				consArgs[pausedConsumers[c]].pause = false;
			}
		}
	}

//	if (prodFluctuations) {
//		for(int i = 0; i < 10; i++) {
//			int prodIdx = (rand()%prodNum);
//			for(int p = 0; p < pausedThreads; p++) {
//				prodArgs[(prodIdx + (p<<1))%prodNum].pause = true;
//			}
//
//			usleep(100*timeToRun);
//
//			for(int p = 0; p < pausedThreads; p++) {
//				prodArgs[(prodIdx + (p<<1))%prodNum].pause = false;
//			}
//		}
//	} else if (consFluctuations) {
//		for(int i = 0; i < 10; i++) {
//			int consIdx = (rand()%consNum);
//			for(int c = 0; c < pausedThreads; c++) {
//				consArgs[(consIdx + (c<<1))%consNum].pause = true;
//			}
//
//			usleep(100*timeToRun);
//
//			for(int c = 0; c < pausedThreads; c++) {
//				consArgs[(consIdx + (c<<1))%consNum].pause = false;
//			}
//		}
//	} else {
//		usleep(1000*timeToRun);
//	}

	cout << "Terminating threads..." << endl;
	syncFlags::stop();
}

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
	
	cout << "producersNum = " << prodNum << " , consumersNum = " << consNum << endl;

	string poolType;
	assert(Configuration::getInstance()->getVal(poolType, "poolType"));
	cout << "poolType is " << poolType << endl;

	bool producerMigrate = true;
	Configuration::getInstance()->getVal(producerMigrate, "producerMigrate");
	cout << "producerMigrate = " << producerMigrate << endl;

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
		consArgs[i].numOfProducers = prodNum;
		consArgs[i].poolPtr = &pools[i];
		consArgs[i].numOfThreads = consNum + prodNum;
		consArgs[i].pause = false;
	}
	
	producerArg* prodArgs = new producerArg[prodNum];
	// value of i will serve as an id for the corresponding producer from now on
	for(int i = 0; i < prodNum; i++)
	{
		prodArgs[i].id = i;
		prodArgs[i].numOfThreads = consNum + prodNum;
		prodArgs[i].pause = false;
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
	cout << "Waiting for pools initialization..." << endl;
	while(syncFlags::getAllocatedPoolsCounter() < consNum){}
	cout << "Pools created successfully..." << endl;
	// set consumer-to-pool mapping in ArchEnvironment
	ArchEnvironment::getInstance()->setConsumerToPoolMapping(pools);
	
	
	void** prodStatsArray = new void*[prodNum];
	void** consStatsArray = new void*[consNum];
	
	run(consNum, consArgs, prodNum, prodArgs);

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
	unsigned long TotalNumOfProducedTasks = 0;
	double TotalInsertionThroughput = 0;
	unsigned long TotalNumOfRetrievedTasks = 0;
	double TotalSystemThroughput = 0;
	unsigned long stealingCounter = 0;
	AtomicStatistics* prodStats = new AtomicStatistics();
	AtomicStatistics* consStats = new AtomicStatistics();
	for(int i = 0; i < prodNum; i++)
	{
		producerStats* stats = (producerStats*)prodStatsArray[i];
		TotalNumOfProducedTasks += stats->numOfProducedTasks;
		TotalInsertionThroughput += stats->producerThroughput;
		prodStats->add(&(stats->atomicStats));
		delete stats;		
	}
	for(int i = 0; i < consNum; i++)
	{
		consumerStats* stats = (consumerStats*)consStatsArray[i];
		TotalNumOfRetrievedTasks += stats->numOfRetrievedTasks;
		TotalSystemThroughput += stats->consumerThroughput;
		consStats->add(&(stats->atomicStats));
		//cout << "Consumer " << i << ": stealingCounter = " << stats->stealingCounter << endl;
		stealingCounter += stats->stealingCounter;
		delete stats;
	}
	
	//cout << "Total number of inserted tasks = " << TotalNumOfProducedTasks << endl;
	cout << "Peak Insertion throughput (tasks/msec) = " << TotalInsertionThroughput << endl;
	//cout << "Total Number of retrieved tasks = " << TotalNumOfRetrievedTasks << endl;
	cout << "System Throughput (tasks/msec) = " << TotalSystemThroughput << endl;
	double avgStealingAttempts = ((double)stealingCounter)/consNum;
	cout << "Average Number of Work Stealing Attempts per Consumer = " << avgStealingAttempts << endl;

//  prodStats->normalize(prodNum);
//  consStats->normalize(consNum);
//	cout << "Average Producer Atomic Statistics:" << endl;
//	prodStats->print();
//	cout << "Average Consumer Atomic Statistics:" << endl;
//	consStats->print();

	cout << "Producer Atomic Statistics:" << endl;
	cout << "\t CAS operations per inserted task: "
			<< ((double)prodStats->getCASTotal())/TotalNumOfProducedTasks << endl;
	cout << "\t CAS failures per inserted task: "
				<< ((double)prodStats->getCASFailures())/TotalNumOfProducedTasks << endl;
	cout << "Consumer Atomic Statistics:" << endl;
	cout << "\t CAS operations per retrieved task: "
			<< ((double)consStats->getCASTotal())/TotalNumOfRetrievedTasks << endl;
	cout << "\t CAS failures per inserted task: "
			<< ((double)consStats->getCASFailures())/TotalNumOfRetrievedTasks << endl;


	cout << "CSV Formatting: " <<
			"PoolType,ProdNum,ConsNum," <<
			"ProdRate,ConsRate,AvgStealingAttempts,ProdCAS,ProdCASFailure,ConsCAS,ConsCASFailure," <<
			"Affinity,ProdMigrate,ProdFluct,ConsFluct,PausedThreads" << endl;

	bool affinity=false;
	Configuration::getInstance()->getVal(affinity, "forceAssignment");
	bool prodFluctuations = false;
	Configuration::getInstance()->getVal(prodFluctuations, "prodFluctuations");
	bool consFluctuations = false;
	Configuration::getInstance()->getVal(consFluctuations, "consFluctuations");
	int pausedThreads=1;
	Configuration::getInstance()->getVal(pausedThreads, "pausedThreads");
	cout << "CSV:" << poolType << "," <<
			prodNum << "," <<
			consNum << "," <<
			TotalInsertionThroughput << "," <<
			TotalSystemThroughput << "," <<
			avgStealingAttempts << "," <<
			((double)prodStats->getCASTotal())/TotalNumOfProducedTasks << "," <<
			((double)prodStats->getCASFailures())/TotalNumOfProducedTasks << "," <<
			((double)consStats->getCASTotal())/TotalNumOfRetrievedTasks << "," <<
			((double)consStats->getCASFailures())/TotalNumOfRetrievedTasks << "," <<
			affinity << "," <<
			producerMigrate << "," <<
			prodFluctuations << "," <<
			consFluctuations << "," <<
			pausedThreads << endl;


	// free allocated memory
	delete prodStats;
	delete consStats;
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
