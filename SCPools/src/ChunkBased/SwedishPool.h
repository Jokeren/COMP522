#ifndef SWEDISHPOOL_H_
#define SWEDISHPOOL_H_

#include "NoFIFOCASPool.h"

// the implementation of the concurrent bags algorithm by Sundell et al. from SPAA'11
class SwedishPool : public NoFIFOCASPool {

public:
	static SwedishPool* getInstance(int numProd);
	static SwedishPool* getInstance();

	SwedishPool(int _numProducers, int consId, bool createSingleton);
	SwedishPool(int _numProducers, int consId);
	virtual ~SwedishPool() {}; // destructor of NoFIFOPool is enough

	virtual OpResult consume(Task*& t, AtomicStatistics* stat);
	SCTaskPool::ProducerContext* getProducerContext(const Producer& prod);
	Task* steal(SCTaskPool* from, AtomicStatistics* stat);

protected:
	Task* stealFromChunk(SPChunk* chunk, SwNode* node, int queueId, int startIdx, AtomicStatistics* stat);
	static SwedishPool* instance;
	static volatile bool locked;

protected:
	DeleteChunkFunc* deleteChunkFunc;
	SPChunk* lastStealingChunk;
	int lastStealingIdx;
	int lastQueueId;
	SwNode* lastStealingNode;
};

#endif /* SWEDISHPOOL_H_ */
