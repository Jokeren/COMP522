
#include "AtomicStatistics.h"

void AtomicStatistics::add(AtomicStatistics* stat)
{
	CAS_totalCount += stat->CAS_totalCount;
	CAS_failuresCount += stat->CAS_failuresCount;
	FetchAndIncCount += stat->FetchAndIncCount;
	NewChunkCount += stat->NewChunkCount;
  DequeueChunkCount += stat->DequeueChunkCount;
}

void AtomicStatistics::normalize(int num)
{
	CAS_totalCount = CAS_totalCount/num;
	CAS_failuresCount = CAS_failuresCount/num;
	FetchAndIncCount = FetchAndIncCount/num;
	NewChunkCount = NewChunkCount/num;
  DequeueChunkCount = DequeueChunkCount/num;
}

void AtomicStatistics::print(){
	cout << "CAS operations: " << CAS_totalCount << endl;
	cout << "CAS failures: " << CAS_failuresCount  << " => " << 100*(double)CAS_failuresCount/CAS_totalCount << " %" << endl;
	cout << "FetchAndInc/Dec operations: " << FetchAndIncCount << endl;
	cout << "NewChunkCount: " << NewChunkCount << endl;
  cout << "DequeueChunkCount: " << DequeueChunkCount << endl;
}



