#include "NoFIFOCASPool.h"
#include "SwLinkedList.h"
#include "hp/hp.h"

using namespace HP;

Task* NoFIFOCASPool::takeTask(SwNode* n, AtomicStatistics* stat) {
	SPChunk* chunk = n->chunk;
	setHP(3, chunk, hpLoc);
	if (n->chunk != chunk || chunk == NULL)
		return NULL;

	Task* task = NULL;
	// go to the end of TAKEN prefix starting from n->consumerIdx
	while(true) {
		chunk->getTask(task, n->consumerIdx+1);
		if (task != TAKEN) break;
		n->consumerIdx++;
		if (n->consumerIdx + 1 == chunk->getMaxSize()) {
			// arrived to the end of the chunk
			reclaimChunk(n, chunk, currentQueueID, stat);
			return NULL;
		}
	}

	if (task == NULL)
		return NULL; // still nothing inserted

	n->consumerIdx++;
	if (chunk->markTaken(n->consumerIdx, task, stat)) {
		if (n->consumerIdx + 1 == chunk->getMaxSize()) {
			reclaimChunk(n, chunk, currentQueueID, stat);
		}
		return task;
	} else {
		return NULL;
	}
}

float NoFIFOCASPool::getStealingScore() const {
	return 1;
}

float NoFIFOCASPool::getStealingThreshold() const {
	return 0;
}

Task* NoFIFOCASPool::steal(SCTaskPool* from_, AtomicStatistics* stat) {
	NoFIFOCASPool* from = (NoFIFOCASPool*)from_;

	int idx = getLongestListIdx();
	return stealFromList(&(from->chunkLists[idx]), stat);
}

Task* NoFIFOCASPool::stealFromList(SwLinkedList* l, AtomicStatistics* stat) {
	// iterate over the list of nodes, try to steal from each one of them
	SwLinkedList::SwLinkedListIterator iter(l,hpLoc);
	SwNode* node;
	while(true) {
		if (iter.next(node) != SUCCESS) {
			// reset it, start from the beginning
			iter.reset(l);
		} else {
			if (node == NULL) {
				// we're at the end of the list
				return NULL;
			} else {
				SPChunk* chunk = node->chunk;
				setHP(3, chunk, hpLoc);
				if (node->chunk != chunk || chunk == NULL) continue; // that was an empty node
				return stealFromChunk(chunk, node->consumerIdx, stat);
			}
		}
	}
}


Task* NoFIFOCASPool::stealFromChunk(SPChunk* chunk, int startIdx, AtomicStatistics* stat) {
	int idx = startIdx;
	Task* t;
	while(true) {
		// iterate to the end of TAKEN values
		for(; idx < chunk->getMaxSize() && chunk->isTaken(idx); idx++) {};
		if (idx >= chunk->getMaxSize()) return NULL; // arrived to the end of the chunk
		chunk->getTask(t, idx);
		if (t == NULL) return NULL; // still no tasks at that idx
		if (t == TAKEN) continue;
		if (chunk->markTaken(idx, t, stat)) {
			// succeeded to steal the task
			return t;
		}
	}
}
