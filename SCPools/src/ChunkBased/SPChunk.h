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
#define TASKS_PER_CHUNK 500


#define TAKEN (Task*)0x1

class SPChunk {
public:
	SPChunk(int owner);
	virtual ~SPChunk();

	virtual OpResult insertTask(const Task*& t);
	virtual OpResult getTask(const Task*& t, int idx);
	virtual bool hasTask(int idx);
	virtual void markTaken(int idx);
	virtual bool markTaken(int idx, const Task* t);
	virtual int getMaxSize();

	virtual int getOwner() const;
	virtual bool changeOwner(int prevOwner, int newOwner);


protected:
	int head; // The index of prefix of the prefix of tasks.
	int owner;
	Task const *tasks[TASKS_PER_CHUNK];
};

#endif /* SPCHUNK_H_ */
