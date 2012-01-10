/*
 * SPChunk.h
 *
 *  Single producer chunk
 *  Created on: Dec 29, 2011
 */

#ifndef SPCHUNK_H_
#define SPCHUNK_H_

//TODO: maybe change to enum?
#define TAKEN (void*)0x1

class SPChunk {
public:
	SPChunk(int owner);
	virtual ~SPChunk();

	virtual OpResult insertTask(const Task& t, bool& isLastTask);
	virtual OpResult getTask(Task*& t, int idx);
	virtual OpResult markTaken(int idx, bool isCAS);

	virtual int getMaxSize();

	virtual int getOwner() const;
	virtual bool changeOwner(int prevOwner, int newOwner);

protected:
	int owner;
};

#endif /* SPCHUNK_H_ */
