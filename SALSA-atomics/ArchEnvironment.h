/*
 * ArchEnvironment.h
 *
 *  Created on: Dec 28, 2011
 */

#ifndef ARCHENVIRONMENT_H_
#define ARCHENVIRONMENT_H_

#include "SCTaskPool.h"
#include "Consumer.h"
#include "Producer.h"
#include "Configuration.h"
#include <map>
#include <list>
#include <stdlib.h>
using namespace std;

class ArchEnvironment {
public:
	static ArchEnvironment* getInstance();

	virtual SCTaskPool** getSortedConsumers(const Producer& prod);
	virtual SCTaskPool** getSortedConsumers(const Consumer& cons);
	int getProducerCore(int tid);
	int getConsumerCore(int tid);
	virtual ~ArchEnvironment();
	void setConsumerToPoolMapping(SCTaskPool** pools){consumerToPool = pools;}
	void threadToCoreChipMapping();

private:
	bool forceAssignment;
	int producersNum;
	int consumersNum;
	int producerCoresNum;
	int* producerCores;
	int consumerCoresNum;
	int* consumerCores;
	int** chips;
	int chipsNum;
	int coresPerChip;
	// These can be replaced with arrays since the producers'/consumers' ids are 0,1,2,...,producersNum/consumersNum -1
	map<int,int>* consumerToCore;
	map<int,int>* consumerToChip;
	map<int,int>* producerToCore;
	map<int,int>* producerToChip;
	SCTaskPool** consumerToPool;
	
	ArchEnvironment();
	static ArchEnvironment* instance;
	string* stringSplit(const string str, const char seperator, int& arrLen);
	int* parseThreadCores(string coresString, int& coreNum);
	int** parseChipCores(string chips, int& chipNum, int& coresPerChip);
	int getChipByCoreId(int id);
	SCTaskPool** getSortedConsumers(int tid, bool producer);
};

#endif /* ARCHENVIRONMENT_H_ */
