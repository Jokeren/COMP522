#ifndef NOFIFOCASPOOL_H_
#define NOFIFOCASPOOL_H_

#include "SCTaskPool.h"
#include "NoFIFOPool.h"
#include "ChunkPool.h"
#include "SwLinkedList.h"
#include "SPChunk.h"

// This class is a straw man for the non-fifo pool
// It uses the same idea of keeping chunks of memory with the following differences:
// 1) every consume uses CAS operations in order to retrieve a task
// 2) the steal operations steal individual tasks insteal of retrieving the whole chunks
class NoFIFOCASPool : public NoFIFOPool {

public:

	NoFIFOCASPool(int _numProducers, int _consumerID) : NoFIFOPool(_numProducers, _consumerID) {};
	virtual ~NoFIFOCASPool() {}; // destructor of NoFIFOPool is enough

	virtual float getStealingScore() const;
	virtual float getStealingThreshold() const;
	virtual Task* steal(SCTaskPool* from, AtomicStatistics* stat);

protected:
	virtual Task* stealFromList(SwLinkedList* l, int listIdx, AtomicStatistics* stat);
	virtual Task* stealFromChunk(SPChunk* n, SwNode* node, int queueId, int startIdx, AtomicStatistics* stat);
	virtual Task* takeTask(SwNode* n, AtomicStatistics* stat);
};

#endif /* NOFIFOCASPOOL_H_ */
