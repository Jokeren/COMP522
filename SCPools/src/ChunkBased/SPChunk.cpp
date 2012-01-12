/*
 * SPChunk.cpp
 *
 *  Created on: Dec 29, 2011
 *      Author: dima39
 */

#include "SPChunk.h"
#include "Atomic.h"


SPChunk::SPChunk(int _owner) : head(0), owner(_owner) {}

SPChunk::~SPChunk() {}


OpResult SPChunk::insertTask(const Task*& t) {
	if (head == TASKS_PER_CHUNK)
		return FULL;
	tasks[head] = t;
	head++;
	return SUCCESS;
}

OpResult SPChunk::getTask(const Task *& t, int idx) {
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

bool SPChunk::markTaken(int idx, const Task* prevTask) {
	return CAS(&(tasks[idx]),prevTask,TAKEN);
}


int SPChunk::getMaxSize() {
	return TASKS_PER_CHUNK;
}

//TODO: implement owner with counter:
int SPChunk::getOwner() const {}
bool SPChunk::changeOwner(int prevOwner, int newOwner) {}


