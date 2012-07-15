/*
 * SPChunk.h
 *
 *  Single producer chunk
 *  Created on: Dec 29, 2011
 */

#ifndef SPCHUNK_H_
#define SPCHUNK_H_

#include "commons.h"
#include "Task.h"
#include "AtomicStatistics.h"

#define TASKS_PER_CHUNK 1000 //1020 // (4KB / 4B) - 2 = 1022


#define TAKEN (Task*)0x1

class SPChunk {
public:
	SPChunk(int owner);
	~SPChunk();

	void clean(); // prepares the chunk for reuse
	bool insertTask(Task* t, bool& lastTask);
	OpResult getTask(Task*& t, int idx);
	bool isTaken(int idx);
	bool hasTask(int idx);
	void markTaken(int idx);
	bool markTaken(int idx, Task* t, AtomicStatistics* stat);
	int getMaxSize();


	static int getOwner(int countedOwner);
	void setOwner(int owner); // will reset the ownership and put the counter to zero
	int getCountedOwner() const;
	bool changeCountedOwner(int prevOwner, int newOwner, AtomicStatistics* stat);


protected:
	int head; // The index for insertion of the next task
	int owner;
	Task *tasks[TASKS_PER_CHUNK];
};

#endif /* SPCHUNK_H_ */
