/*
 * NoFIFOPool.cpp
 *
 *  Created on: Dec 29, 2011
 */

#include "NoFIFOPool.h"
#include "../Configuration.h"
#include "../commons.h"
#include "../Atomic.h"
#include "SPChunk.h"
#include <assert.h>


NoFIFOPool::NoFIFOPool(int _numProducers, int _consumerID) : SCTaskPool(_numProducers), consumerID(_consumerID), currentNode(NULL), currentQueueID(-1)
{

	int initialPoolSize;
	if (!Configuration::getInstance()->getVal(initialPoolSize, "initialPoolSize")) {
		initialPoolSize = 200;
	}

	chunkPool = new ChunkPool(consumerID,initialPoolSize);
	chunkLists = new SwLinkedList[_numProducers+1];
	chunkListSizes = new unsigned int[numProducers+1];
}

NoFIFOPool::~NoFIFOPool() {
	// TODO Auto-generated destructor stub
}



//TODO: implement:
SCTaskPool::ProducerContext* NoFIFOPool::getProducerContext(const Producer& prod) {}

OpResult NoFIFOPool::consume(Task*& t) {
	if (currentNode != NULL) {
		Task* res = takeTask(currentNode);
		if (res != NULL){ t = res; return SUCCESS;}
	}

	//Go over the chunks list in a fair way
	int prevQueueIdx = currentQueueID;

	currentQueueID = (currentQueueID+1) % numProducers;
	SwLinkedList::SwLinkedListIterator* iter = new SwLinkedList::SwLinkedListIterator(NULL);
	while (currentQueueID != prevQueueIdx) {
		//TODO: use list counters instead of the lists
		SwLinkedList currList = chunkLists[currentQueueID];
		iter->reset(&currList);
		SwNode* n = NULL;
		OpResult res;
		while ((res = iter->next(n)) != FAILURE && n != NULL) {
			Task* res = takeTask(n);
			if (res != NULL){
				t = res;
				currentNode = n;
				return SUCCESS;
			}
		}
		if (res == FAILURE) continue;
		currentQueueID = (currentQueueID+1) % numProducers;
	}

	currentNode = NULL;
	t = NULL;
	return EMPTY;


}

float NoFIFOPool::getStealingScore() const {}
float NoFIFOPool::getStealingThreshold() const {}


Task* NoFIFOPool::steal(SCTaskPool& from) {}

int NoFIFOPool::getEmptynessCounter() const {}




NoFIFOPool::ProdCtx::ProdCtx(SwLinkedList& l, unsigned int& c, NoFIFOPool& _noFIFOPool, int _producerId)
 : chunkList(l),
   chunkCount(c),
   curChunk(NULL),
   noFIFOPool(_noFIFOPool),
   producerId(_producerId)
{
	curChunk = noFIFOPool.chunkPool->getChunk();
	chunkList.append(curChunk);
	FAA(&chunkCount, 1);
}

OpResult NoFIFOPool::ProdCtx::produce(const Task& t, bool& changeConsumer) {
	return produceAux(t, changeConsumer);
}

void NoFIFOPool::ProdCtx::produceForce(const Task& t) {
	bool dummy;
	produceAux(t, dummy, true);
}

OpResult NoFIFOPool::ProdCtx::produceAux(const Task& t, bool& changeConsumer, bool force) {
	OpResult res = curChunk->insertTask(t, changeConsumer);
	if (res == SUCCESS) {
		return SUCCESS;
	}
	// the previous chunk is full
	// Try to get a new chunk from the chunk pool
	SPChunk* newChunk = noFIFOPool.chunkPool->getChunk();
	if (newChunk == NULL) {
		// no free chunks in the pool
		if (!force)	{
			return FULL;
		} else {
			newChunk = new SPChunk(producerId);
		}
	}
	chunkList.append(newChunk);
	FAA(&chunkCount, 1);
	curChunk = newChunk;
	assert(curChunk->insertTask(t, changeConsumer) == SUCCESS); // cannot fail
	return SUCCESS;
}


//AUX
Task* NoFIFOPool::takeTask(SwNode* n) {
	SPChunk* chunk = n->chunk;
	if (chunk == NULL) return NULL;
	Task* task = NULL;
	chunk->getTask(task,n->consumerIdx+1);
	if (task == NULL) return NULL;

	n->consumerIdx++;
	if (chunk->getOwner() == consumerID) {
		//fast path:
		chunk->markTaken(n->consumerIdx,false);
		if (n->consumerIdx + 1 == chunk->getMaxSize()) {
			FAA(&(chunkListSizes[currentQueueID]),-1);
			n->chunk = NULL;
			currentNode = NULL;
			//TODO: reclaim chunk
		}
		return task;
	}
	//chunk was stolen:
	FAA(&(chunkListSizes[currentQueueID]),-1);
	bool success = (task != TAKEN && chunk->markTaken(n->consumerIdx,true) == SUCCESS);
	n->chunk = NULL;
	currentNode = NULL;
	return success ? task : NULL;


}

//	NoFIFOPool(int _numProducers);
//	virtual ~NoFIFOPool();
//
//	virtual ProducerContext* getProducerContext(const Producer& prod);
//	virtual OpResult consume(Task*& t);
//
//	virtual float getStealingScore() const;
//	virtual float getStealingThreshold() const;
//	virtual Task* steal(SCTaskPool& from);
//
//	virtual int getEmptynessCounter() const;
