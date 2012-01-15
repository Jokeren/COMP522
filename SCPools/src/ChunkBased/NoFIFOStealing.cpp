#include "NoFIFOPool.h"

#include "hp/hp.h"
using namespace HP;


float NoFIFOPool::getStealingScore() const {
	return 0;
}
float NoFIFOPool::getStealingThreshold() const {
	return 0;
}


// Gets a non-empty node from the longest list or NULL if no node exists
// prefer a non-stolen node, unless no list is bigger than 1.
// HP #2 will point to that node SP 3 will point to the chunk.
//TODO: check if this is a good way to choose chunks
SwNode* NoFIFOPool::getStealNode(int &stealQueueID) {
	return NULL; // TODO: need to talk with Elad, I can't understand what's going on here
//	SwLinkedList::SwLinkedListIterator iter(NULL);
//	SwNode *n1 = NULL, *n2 = NULL;
//	HPLocal hpLoc = getHPLocal();
//	SPChunk* c = NULL;
//	while (true) {
//		//find list with max size:
//		unsigned int max = 0;
//		int maxIdx = -1;
//		for (int i = 0; i < numProducers; i++) {
//			if (chunkListSizes[i] > max) {
//				max = chunkListSizes[i];
//				maxIdx = i;
//			}
//		}
//
//		if (max < 1 && chunkListSizes[numProducers] != 0) {
//			stealQueueID = numProducers;
//			//no list with more than 1 chunk - try to steal any node from steal list
//			SwLinkedList& list = chunkLists[numProducers];
//			iter.reset(&list);
//			if (iter.next(n1) == FAILURE)
//				continue;
//			if (n1 != NULL) {
//				c = n1->chunk;
//				setHP(3, c, hpLoc);
//				if (n1->chunk != c)
//					continue;
//				if (c != NULL && c->hasTask(n1->consumerIdx + 1)) {
//					setHP(2, n1, hpLoc);
//					return n1;
//				}
//			}
//		}
//		if (max == 0)
//			return NULL;
//
//		// steal from longest list, try to steal the second chunk.
//		// steal the first chunk if there is no second chunk.
//		SwLinkedList& list = chunkLists[maxIdx];
//		stealQueueID = maxIdx;
//
//		iter.reset(&list);
//		if (iter.next(n1) == FAILURE)
//			continue;
//		//TODO: may just return NULL here:
//		if (n1 == NULL)
//			continue;
//		setHP(2, n1, hpLoc);
//		if (iter.next(n2) == FAILURE)
//			continue;
//		if (n2 != NULL) {
//			c = n2->chunk;
//			setHP(3, c, hpLoc);
//			if (n2->chunk != c)
//				continue;
//			if (c != NULL && c->hasTask(n2->consumerIdx + 1)) {
//				setHP(2, n2, hpLoc);
//				return n2;
//			}
//		}
//		c = n1->chunk;
//		setHP(3, c, hpLoc);
//		if (n1->chunk != c)
//			continue;
//		if (c != NULL && c->hasTask(n1->consumerIdx + 1)) {
//			return n1;
//		}
//	}
}

Task* NoFIFOPool::steal(SCTaskPool* from_, AtomicStatistics* stat) {
	NoFIFOPool* from = (NoFIFOPool*)from_;
	// obtain a non-empty node or NULL if there are no nodes.
	int stealQueueID = -1;
	SwNode* prevNode = from->getStealNode(stealQueueID);
	if (prevNode == NULL)
		return NULL;

	SPChunk* c = prevNode->chunk; // TODO: do we need hazard pointers here?
	if (c == NULL)
		return NULL;

	// make the prevNode restealable by adding it to my own stealList
	SwLinkedList& stealList = chunkLists[numProducers];
	stealList.append(prevNode);
	// try to change the owner
	int prevOwner = c->getCountedOwner();
	if (SPChunk::getOwner(prevOwner) == from->consumerID ||
			(c->changeCountedOwner(prevOwner, this->consumerID) == false)) {
		// didn't succeed to change the owner (either it didn't correspond to the id of the from list
		// or it has been already stolen by someone else
		stealList.remove(prevNode);
		return NULL;
	}

	// succeeded to steal, create the copy of the node and put it in the stealing list
	SwNode* newNode = new SwNode(c);
	newNode->consumerIdx = prevNode->consumerIdx;
	stealList.replace(prevNode, newNode);
	prevNode->chunk = NULL;

	// update counters for stealer and the one we stole from
	FAA(&(chunkListSizes[numProducers]), 1);
	FAA(&(from->chunkListSizes[stealQueueID]), -1);

	// try to grab the task from the stolen chunk (in order to guarantee progress)
	int idx = newNode->consumerIdx;
	Task* task = NULL;
	c->getTask(task, idx + 1);
	if (task == NULL) return NULL;
	if (task == TAKEN || !c->markTaken(idx + 1, task)) {
		task = NULL;
	} else if (idx + 2 == c->getMaxSize()) {
		// stolen the last task in the chunk
		reclaimChunk(newNode, c, numProducers);
	}
	newNode->consumerIdx++;
	return task;
}

