/*
 * SPChunk.cpp
 *
 *  Created on: Dec 29, 2011
 *      Author: dima39
 */

#include "SPChunk.h"
#include "Atomic.h"
#include <string.h>
#include <assert.h>
#include <iostream>

using namespace std;


// we will use 10 bits for the ownership and all the others for the counter
const int OWNER_MASK = ((1 << 10) - 1);
const int COUNTER_MASK = ~((1 << 10) - 1);
const int OWNER_SHIFT = 0;
const int COUNTER_SHIFT = 10;

SPChunk::SPChunk(int _owner) : head(0), owner(_owner) {
	clean();
}

SPChunk::~SPChunk() {}

void SPChunk::clean() {
	head = 0;
	// reset the array of task pointers
	memset(tasks, 0, TASKS_PER_CHUNK*sizeof(Task*));
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

bool SPChunk::isTaken(int idx) {
	return (tasks[idx] == TAKEN);
}

void SPChunk::markTaken(int idx) {
	tasks[idx] = TAKEN;
}

bool SPChunk::markTaken(int idx, Task* prevTask, AtomicStatistics* stat) {
	assert(prevTask != TAKEN);
	stat->CAS_count_inc();
	bool res = CAS(&(tasks[idx]),prevTask,TAKEN);
	if (!res) {
		stat->CAS_failures_inc();
	}
	return res;
}


int SPChunk::getMaxSize() {
	return TASKS_PER_CHUNK;
}

void SPChunk::setOwner(int o) {
	// will reset the ownership and put the counter to zero
	owner = o;
}

int SPChunk::getOwner(int countedOwner) {
	return (countedOwner & OWNER_MASK);
}

int SPChunk::getCountedOwner() const {
	return owner;
}

bool SPChunk::changeCountedOwner(int prevOwner, int newOwner, AtomicStatistics* stat) {
	if (owner != prevOwner) return false;

	int counter = (prevOwner & COUNTER_MASK) >> COUNTER_SHIFT;
	int newVal = ((newOwner & OWNER_MASK) | ((counter+1) << COUNTER_SHIFT));
	bool res = CAS(&owner, prevOwner, newVal);
	stat->CAS_count_inc();
	if (!res) {
		stat->CAS_failures_inc();
	}
	return res;
}


