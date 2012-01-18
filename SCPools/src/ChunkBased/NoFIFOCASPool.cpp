#include "NoFIFOCASPool.h"
#include "SwLinkedList.h"
#include "hp/hp.h"

using namespace HP;

Task* NoFIFOCASPool::takeTask(SwNode* n) {
	HPLocal hpLoc = getHPLocal();
	SPChunk* chunk = n->chunk;
	setHP(3, chunk, hpLoc);
	if (n->chunk != chunk || chunk == NULL)
		return NULL;

	Task* task = NULL;
	// find the last TAKEN task starting from n->consumerIdx
	while(true) {
		chunk->getTask(task, n->consumerIdx+1);
		if (task != TAKEN) break;
		n->consumerIdx++;
		if (n->consumerIdx + 1 == chunk->getMaxSize()) {
			// arrived to the end of the chunk
			reclaimChunk(n, chunk, currentQueueID);
			return NULL;
		}
	}

	chunk->getTask(task, n->consumerIdx + 1);
	if (task == NULL)
		return NULL; // still nothing inserted

	n->consumerIdx++;
	if (chunk->markTaken(n->consumerIdx, task) == SUCCESS) {
		if (n->consumerIdx + 1 == chunk->getMaxSize()) {
			reclaimChunk(n, chunk, currentQueueID);
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

	// iterate over the lists, try to steal from each one of them
	for(int i = 0; i < from->numProducers+1; i++) {
		Task* t = stealFromList(&(from->chunkLists[i]));
		if (t != NULL) return t;
	}
	return NULL;
}

Task* NoFIFOCASPool::stealFromList(SwLinkedList* l) {
	HPLocal hpLoc = getHPLocal();
	// iterate over the list nodes, try to steal from each one of them
	SwLinkedList::SwLinkedListIterator iter(l);
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
				if (chunk == NULL || node->chunk == NULL) continue; // that was an empty node
				Task *t = stealFromChunk(chunk);
				if (t != NULL) return t;
			}
		}
	}
}


Task* NoFIFOCASPool::stealFromChunk(SPChunk* chunk) {
	int idx = 0;
	Task* t;
	while(true) {
		// iterate to the end of TAKEN values
		for(; idx < chunk->getMaxSize() && !chunk->isTaken(idx); idx++) {};
		if (idx >= chunk->getMaxSize()) return NULL; // arrived to the end of the chunk
		chunk->getTask(t, idx);
		if (t == NULL) return NULL; // still no tasks at that idx

		if (chunk->markTaken(idx, t) == SUCCESS) {
			// succeeded to steal the task
			return t;
		}
	}
}
