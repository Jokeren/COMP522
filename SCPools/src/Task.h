#ifndef TASK_H_
#define TASK_H_

#include <stddef.h>

class Task{
public:
	virtual void* run(void* arg) = 0;
};

class DummyTask : public Task {
protected:
	long Id;
public:
	/* insertion time fields. updated by the producer prior to insertion into container */
	unsigned long insertionTime_sec;
	unsigned long insertionTime_ns;
	
	/* constructors */
	DummyTask(): Id(0), insertionTime_sec(0), insertionTime_ns(0) {}
	DummyTask(int id) {
		this->Id = id;
		insertionTime_sec = insertionTime_ns = 0;
	}
	
	long getId() {
		return Id;
	}
	
	void* run(void* arg) {
		/*int p = 0;
		for(int i = 0; i < 5000; i++)
		{
			p += i;
		}*/
		return NULL;
	}
	
	virtual ~DummyTask() {}
};
#endif
