/*
 * SPChunk.cpp
 *
 *  Created on: Dec 29, 2011
 *      Author: dima39
 */

#include "SPChunk.h"
#include "Atomic.h"

// we will use 10 bits for the ownership and all the others for the counter
const int OWNER_MASK = ((1 << 10) - 1);
const int COUNTER_MASK = ~((1 << 10) - 1);
const int OWNER_SHIFT = 0;
const int COUNTER_SHIFT = 10;

SPChunk::SPChunk(int _owner) : head(0), owner(_owner) {}

SPChunk::~SPChunk() {}

void SPChunk::clean() {
	head = 0;
	// reset the counter
	owner &= OWNER_MASK;
	// reset the array of task pointers
	for(int i = 0; i < TASKS_PER_CHUNK; i++) {
		tasks[i] = NULL;
	}
}


bool SPChunk::insertTask(Task* t, bool& lastTask) {
	if (head == TASKS_PER_CHUNK)
		return false;
	tasks[head++] = t;
	lastTask = (head == TASKS_PER_CHUNK);
	return true;
}

OpResult SPChunk::getTask(Task *& t, int idx) {
	t = tasks[idx];
	return SUCCESS;
}

bool SPChunk::hasTask(int idx) {
	if (tasks[idx] == NULL || tasks[idx] == TAKEN)
		return false;
	return true;
}

void SPChunk::markTaken(int idx) {
	tasks[idx] = TAKEN;
}

bool SPChunk::markTaken(int idx, Task* prevTask) {
	return CAS(&(tasks[idx]),prevTask,TAKEN);
}


int SPChunk::getMaxSize() {
	return TASKS_PER_CHUNK;
}


int SPChunk::getOwner(int countedOwner) {
	return (countedOwner & OWNER_MASK);
}

int SPChunk::getCountedOwner() const {
	return owner;
}

bool SPChunk::changeCountedOwner(int prevOwner, int newOwner) {
	if (owner != prevOwner) return false;

	int counter = (prevOwner & COUNTER_MASK) >> COUNTER_SHIFT;
	int newVal = ((newOwner & OWNER_MASK) | ((counter+1) << COUNTER_SHIFT));
	return CAS(&owner, prevOwner, newVal);
}


