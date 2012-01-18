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

//TODO: const for now, maybe changed later.
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
	bool markTaken(int idx, Task* t);
	int getMaxSize();


	static int getOwner(int countedOwner);
	int getCountedOwner() const;
	bool changeCountedOwner(int prevOwner, int newOwner);


protected:
	int head; // The index for insertion of the next task
	int owner;
	Task *tasks[TASKS_PER_CHUNK];
};

#endif /* SPCHUNK_H_ */
