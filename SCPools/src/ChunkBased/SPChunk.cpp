/*
 * SPChunk.cpp
 *
 *  Created on: Dec 29, 2011
 *      Author: dima39
 */

#include "SPChunk.h"

SPChunk::SPChunk(int owner)  {
	// TODO Auto-generated constructor stub

}

SPChunk::~SPChunk() {
	// TODO Auto-generated destructor stub
}

//TODO: implement

OpResult SPChunk::insertTask(const Task& t, bool& isLastTask) {}

OpResult SPChunk::getTask(Task *& t, int idx){}

bool SPChunk::hasTask(int idx) {}

OpResult SPChunk::markTaken(int idx, bool isCAS){}

int SPChunk::getMaxSize() {}

int SPChunk::getOwner() const {}

bool SPChunk::changeOwner(int prevOwner, int newOwner) {}


