/*
 * ArchEnvironment.cpp
 *
 *  Created on: Dec 28, 2011
 */

#include "ArchEnvironment.h"
#include <assert.h>

ArchEnvironment* ArchEnvironment::instance = NULL;

ArchEnvironment* ArchEnvironment::getInstance() {
	if (instance == NULL) {
		instance = new ArchEnvironment();
	}
	return instance;
}

ArchEnvironment::ArchEnvironment() {
	string consString, prodString, chipString;
	assert(Configuration::getInstance()->getVal(consString, "cons"));
	assert(Configuration::getInstance()->getVal(prodString, "prods"));
	assert(Configuration::getInstance()->getVal(chipString, "chips"));
	assert(Configuration::getInstance()->getVal(producersNum, "producersNum"));
	assert(Configuration::getInstance()->getVal(consumersNum, "consumersNum"));
	consumerCores = parseThreadCores(consString, consumerCoresNum);
	producerCores = parseThreadCores(prodString, producerCoresNum);
	chips = parseChipCores(chipString, chipsNum, coresPerChip);
	consumerToPool = NULL;
}

string* ArchEnvironment::stringSplit(const string str, const char seperator, int& arrLen)
{
	arrLen = 0;
	int strLen = (int)str.length();
	if(strLen == 0){return NULL;}
	for(int i = 0; i < strLen; i++)
	{
		if(str[i] == seperator){arrLen++;}
	}
	arrLen++;
	string* res = new string[arrLen];  
	int pos = 0;
	int i = 0;
	bool finished = false;
	while(!finished)
	{
		int new_pos = str.find(seperator,pos);
		if(new_pos == (int)string::npos)
		{
			new_pos = strLen;
			finished = true;
		}
		res[i] = str.substr(pos,new_pos-pos);
		pos = new_pos+1;
		i++;
	}
	return res;
}


int* ArchEnvironment::parseThreadCores(string coresString, int& coreNum)
{
	int arrLen = 0;
	string* strCores = stringSplit(coresString, ' ', arrLen);
	int* cores = new int[arrLen];  
	for(int i = 0; i < arrLen; i++)
	{
		cores[i] = atoi(strCores[i].c_str());
	}
	coreNum = arrLen;
	delete[] strCores;
	return cores;
}

int** ArchEnvironment::parseChipCores(string chips, int& chipNum, int& coresPerChip)
{
	string* chipsArr = stringSplit(chips,':',chipNum);
	int** chipCores = new int*[chipNum];  
	for(int i = 0; i < chipNum; i++)
	{
		string* procs = stringSplit(chipsArr[i],' ',coresPerChip);
		chipCores[i] = new int[coresPerChip];  
		for(int j = 0; j < coresPerChip; j++)
		{
			chipCores[i][j] = atoi(procs[j].c_str());
		}
		delete[] procs;
	}
	delete[] chipsArr;
	return chipCores;
}

int ArchEnvironment::getChipByCoreId(int coreId)
{
	for(int i = 0; i < chipsNum; i++)
	{
		for(int j = 0; j < coresPerChip; j++)
		{
			if(chips[i][j] == coreId){return i;}
		}
	}
	return -1;
}

void ArchEnvironment::threadToCoreChipMapping(){
	producerToChip = new map<int,int>();
	producerToCore = new map<int,int>();
	consumerToChip = new map<int,int>();
	consumerToCore = new map<int,int>();
	// i is the consumer's id
	for(int i = 0; i < consumersNum; i++)
	{
		int index = i % consumerCoresNum;
		int core = consumerCores[index];
		int chipId = getChipByCoreId(core);
		consumerToCore->insert(pair<int,int>(i,core));
		consumerToChip->insert(pair<int,int>(i,chipId));
	}
	// i is the producer's id
	for(int i = 0; i < producersNum; i++)
	{
		int index = i % producerCoresNum;
		int core = producerCores[index];
		int chipId = getChipByCoreId(core);
		producerToCore->insert(pair<int,int>(i,core));
		producerToChip->insert(pair<int,int>(i,chipId));
	}	
}

SCTaskPool** ArchEnvironment::getSortedConsumers(int tid, bool producer){
	int* resIds = new int[consumersNum];
	bool* taken = new bool[consumersNum];
	for(int i = 0; i < consumersNum; i++)
	{
		resIds[i] = -1;
		taken[i] = false;
	}
	int coreId,chipId;
	if(producer)
	{
		coreId = producerToCore->find(tid)->second;
		chipId = producerToChip->find(tid)->second;
	}
	else
	{
		coreId = consumerToCore->find(tid)->second;
		chipId = consumerToChip->find(tid)->second;
	}
	
	int insertIdx = 0;
	for(int i = 0; i < consumersNum; i++)
	{
		int next = (tid + i) % consumersNum;
		if(consumerToCore->find(next)->second == coreId)
		{
			taken[next] = true;
			resIds[insertIdx] = next;
			insertIdx++;
		}
	}
	for(int i = 0; i < consumersNum; i++)
	{
		int next = (tid + i) % consumersNum;
		if(!taken[next] && consumerToChip->find(next)->second == chipId)
		{
			taken[next] = true;
			resIds[insertIdx] = next;
			insertIdx++;
		}
	}
	for(int i = 0; i < consumersNum; i++)
	{
		int next = (tid + i) % consumersNum;
		if(!taken[next])
		{
			taken[next] = true;
			resIds[insertIdx] = next;
			insertIdx++;
		}
	}
	
	SCTaskPool** res = new SCTaskPool*[consumersNum];
	for(int i = 0; i < consumersNum; i++)
	{
		res[i] = consumerToPool[resIds[i]];
	}
	/***** debug ****/
	/*
	if(!producer && tid == 8)
	{
		for(int i = 0; i < consumersNum; i++)
		{
			cout << resIds[i] << " ";
		}
		cout << endl;
	}
	*/
	/*****************/
	return res;	
}

SCTaskPool** ArchEnvironment::getSortedConsumers(const Producer& prod){
	int id = prod.getId();
	SCTaskPool** res = getSortedConsumers(id,true);
	return res;
}

SCTaskPool** ArchEnvironment::getSortedConsumers(const Consumer& cons){
	int id = cons.getId();
	SCTaskPool** res = getSortedConsumers(id,false);
	return res;
}

int ArchEnvironment::getConsumerCore(int tid){
	return consumerToCore->find(tid)->second;
}

int ArchEnvironment::getProducerCore(int tid){
	return producerToCore->find(tid)->second;
}

ArchEnvironment::~ArchEnvironment(){
	delete[] producerCores;
	delete[] consumerCores;
	for(int i = 0; i < chipsNum; i++)
	{
		delete[] chips[i];
	}
	delete[] chips;
	delete consumerToCore;
	delete consumerToChip;
	delete producerToCore;
	delete producerToChip;
}

	
