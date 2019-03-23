/*
	AtomicStatistics class holds counters for Atomic operations statistics
*/
#ifndef ATOMICSTATISTICS_H_
#define ATOMICSTATISTICS_H_
#include <iostream>
using namespace std;

class AtomicStatistics{
	unsigned long CAS_totalCount;
	unsigned long CAS_failuresCount;
	unsigned long FetchAndIncCount;

public:
	AtomicStatistics():CAS_totalCount(0),CAS_failuresCount(0),FetchAndIncCount(0){}

	void CAS_count_inc(){CAS_totalCount++;}

	void CAS_failures_inc(){CAS_failuresCount++;}
	
	void FetchAndIncCount_inc(){FetchAndIncCount++;}

	/* add fields of another Statistics object */
	void add(AtomicStatistics* stat);

	/* divide counters by num */
	void normalize(int num);

	unsigned long getCASTotal() {
		return CAS_totalCount;
	}

	unsigned long getCASFailures() {
		return CAS_failuresCount;
	}

	unsigned long getFAA() {
		return FetchAndIncCount;
	}

	void print();
};
#endif /* ATOMICSTATISTICS_H_ */
