/*
 * hp.c
 *  A straightforward hazard pointer implementation.
 *
 *  Implemented as described in:
 *  "Hazard Pointers: Safe Memory Reclamation for Lock-Free Objects"
 *  (http://www.research.ibm.com/people/m/michael/ieeetpds-2004.pdf)
 *
 *      Author: Elad Gidron
 */

#include "hp.h"
#include "../Atomic.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

namespace HP {

typedef struct stack_t StackT, *Stack;

typedef struct recliam_data_t {
	void* address;
	void* reclaimationFunc;
} ReclaimationData;

struct stack_t{
	int rcount;
	int size;
	ReclaimationData* data;
};

// Returns new stack with size = initialSize
Stack stackInit(int initialSize) {
	Stack s = (Stack)malloc(sizeof(StackT));
	assert(s != NULL);
	s->data = (ReclaimationData*)malloc(sizeof(ReclaimationData) * initialSize);
	assert(s->data != NULL);
	s->size = initialSize;
	s->rcount = 0;
	return s;
}

//Adds obj to stack. may resize the stack if needed.
void stackPush(Stack stack, void* addr, void* reclaimationFunc) {
	//resize if needed
	if (stack->rcount == stack->size - 1) {
		stack->data = (ReclaimationData *)realloc(stack->data,sizeof(ReclaimationData) * stack->size*2);
		stack->size *= 2;
	}
	//push obj
	stack->data[stack->rcount].address = addr;
	stack->data[stack->rcount].reclaimationFunc = reclaimationFunc;
	stack->rcount++;
}

// removes head of stack and returns it.
// returns NULL if stack is empty.
ReclaimationData* stackPop(Stack stack) {
	if (stack->rcount == 0) return NULL;
	ReclaimationData* res = &(stack->data[--stack->rcount]);
	return res;
}

// Returns the number of elements in the stack
int stackCount(Stack stack) {
	return stack->rcount;
}

// Resets the stack
void stackReset(Stack stack) {
	stack->rcount = 0;
}

// Frees stacks' resources
void stackDestory(Stack stack) {
	free(stack->data);
	free(stack);
}

typedef struct set_t *Set, SetT;

struct set_t {
	int last;		// index of the last object in the set
	int isSorted;	// flag to determine if the data array is sorted
	int maxSize;	// max set size
	long* data;		// the data array
};

//AUX
void bubbleSort(long* array, int size);
int binarySearch(long array[], int Size, long value);

// Returns a set with maximun size of maxSize.
Set setInit(int maxSize) {
	int i = 0;
	Set s = (Set)malloc(sizeof(SetT));
	assert(s != NULL);
	s->data = (long*)malloc(sizeof(long) * maxSize);
	assert(s->data != NULL);
	for (i = 0; i < maxSize; i++) {
		s->data[i] = 0;
	}
	s->last = 0;
	s->isSorted = 1;
	s->maxSize = maxSize;
	return s;
}

// Adds value val to set.
void setAdd(Set set, long val) {
	assert(set->last < set->maxSize);
	set->isSorted = 0;
	set->data[set->last] = val;
	set->last++;
}

// Returns TRUE if val is in set, otherwise returns false.
int setContains(Set set, long val) {
	if (!set->isSorted) {
		bubbleSort(set->data,set->last);
	}
	return binarySearch(set->data,set->last,val);
}

// Resets the set.
void setReset(Set set) {
	set->isSorted = 1;
	set->last=0;
}

//Frees the Set's resources.
void setDestroy(Set set) {
	free(set->data);
	free(set);
}


/****** AUX functions ******/

void bubbleSort(long* array, int size)
{
   int swapped;
   int i;
   for (i = 1; i < size; i++)
   {
       swapped = 0;    //this flag is to check if the array is already sorted
       int j;
       for(j = 0; j < size - i; j++)
       {
           if(array[j] > array[j+1])
           {
        	   long temp = array[j];
               array[j] = array[j+1];
               array[j+1] = temp;
               swapped = 1;
           }
       }
       if(!swapped){
           break; //if it is sorted then stop
       }
   }
}

int binarySearch(long array[], int Size, long value)
{
    int low = 0, high = Size - 1, midpoint = 0;
    while (low <= high)
    {
            midpoint = low + (high - low)/2;
            if (value == array[midpoint])
            {
                    return 1;
            }
            else if (value < array[midpoint])
                    high = midpoint - 1;
            else
                    low = midpoint + 1;
    }
    return 0;
}

//Global vars

//Head of HPrecord list - needed for thread register
AtomicWrapper<HPRecord *> head;
//The per thread data
__thread HPLocal localHPData;

// AUX ///
void SwapStacks(Stack* s1, Stack* s2);
void scan(HPLocal localData);
void prepareForReuse(void* data);
///////

/*** Data structures ****/
//Data structures
//A struct used to initialize the hazard pointers system
//generated by initHPData()
struct HPData_t {
	int HP_COUNT; //The number of hazard pointers per thread
	int THREAD_COUNT; //Total number of threads
	int REC_COUNT; //Number of elements that can be free'd before scan() is called.
};

//A record that holds the hazard pointers of each thread.
struct HPRecord_t {
  AtomicWrapper<struct HPRecord_t *> next; //Next record in the list.
	void* hp[]; //The hazardextern pointers
};

//Hold local data structures needed for each thread.
struct HPLocal_t {
	HPRecord* HPRecHead; //head of the HPRecord list
	HPRecord* localRecord; //The current thread HPRecord
	Stack rlist; //List of retired nodes that were not free'd
	Stack temp; //Stack for internal usage
	Set plist; //Set for internal usage
	HPData hpData; //The HP parameters.
};

/*** External functions ***/

HPData initHPData(int hpCountPerThread, int ThreadCount, int recCount) {
	HPData res = (HPData) malloc(sizeof(struct HPData_t));
	assert(res != NULL);
	res->HP_COUNT = hpCountPerThread;
	res->THREAD_COUNT = ThreadCount;
	if (recCount == -1)
		res->REC_COUNT = hpCountPerThread*ThreadCount;
	else
		res->REC_COUNT = recCount;
	return res;
}

HPLocal getHPLocal() {
	assert(localHPData);
	return localHPData;
}

/*
 * Adds a thread to the HPReord list.
 * hpData - An HPData object created by initHPData
 */
void threadRegister(HPData hpData) {
	int i;
	//init record
	HPRecord* record = (HPRecord*) malloc(sizeof(HPRecord) + (sizeof(void*)
			* hpData->HP_COUNT));
	assert(record != NULL);
  record->next.store(NULL);
	for (i = 0; i < hpData->HP_COUNT; i++) {
		record->hp[i] = NULL;
	}

	//init thread local struct
	HPLocal res = (HPLocal) malloc(sizeof(HPLocalT));
	assert(res != NULL);

	res->localRecord = record;

	int stackSize = hpData->THREAD_COUNT * hpData->HP_COUNT
					> hpData->REC_COUNT ? hpData->THREAD_COUNT
					* hpData->HP_COUNT : hpData->REC_COUNT;

	res->rlist = stackInit(stackSize);
	res->temp = stackInit(stackSize);
	res->plist = setInit(stackSize);
	res->hpData = hpData;

  HPRecord *mhead = NULL;
	if (head.load() == NULL && head.compare_exchange_strong(mhead,record)) {
		res->HPRecHead = record;
	} else {
		res->HPRecHead = head.load();
		//add record to list
		while (1) {
			HPRecord* last = head.load();
			while (last->next.load() != NULL)
				last = last->next.load();
      mhead = NULL;
			if (last->next.compare_exchange_strong(mhead, record))
				break;
		}
	}
	localHPData = res;
}

void resetHPSystem() {
	HPRecord* curr = head.load();
	if (curr == NULL)
		return;
	while (curr->next.load() != NULL) {
		HPRecord* tmp = curr;
    curr = curr->next.load();
		free(tmp);
	}
	head.store(NULL);
}

// Marks that the node needs to be freed and calls scan if needed.
void retireNode(void* addr, ReclaimationFunc* reclaimationFunc,
		HPLocal localData) {
	stackPush(localData->rlist, addr, reclaimationFunc);
	if (stackCount(localData->rlist) >= localData->hpData->REC_COUNT) {
		scan(localData);
	}
}

void setHP(int idx, void* addr, HPLocal localData) {
	if (idx >= localData->hpData->HP_COUNT) {
		printf("IDX: %d max: %d \n", idx, localData->hpData->HP_COUNT);
		assert(0);
	}
	localData->localRecord->hp[idx] = addr;
}

HPRecord* getHPListHead(HPLocal localData) {
	return localData->HPRecHead;
}

// Goes over the list of nodes that needs to be freed
// and frees them if no HP points at them.
void scan(HPLocal localData) {
	int i;
	//Stage 1: scan HP list and insert non-null values to plist
	Set plist = localData->plist;
	setReset(plist);
	HPRecord* curr = localData->HPRecHead;
	while (curr != NULL) {
		for (i = 0; i < localData->hpData->HP_COUNT; i++) {
			if (curr->hp[i] != NULL) {
				setAdd(plist, (long) (curr->hp[i]));
			}
		}
    curr = curr->next.load();
	}
	//Stage 2: search plist
	//uses two stacks instead of allocating a new one each time scan() is called
	SwapStacks(&localData->rlist, &localData->temp);
	stackReset(localData->rlist);
	ReclaimationData* recData = stackPop(localData->temp);
	while (recData != NULL) {
		if (setContains(plist, (long) recData->address)) {
			stackPush(localData->rlist, recData->address,
					recData->reclaimationFunc);
		} else {
				(*((ReclaimationFunc*)recData->reclaimationFunc))(recData->address);
		}
		recData = stackPop(localData->temp);
	}
	setReset(plist);
}

int getHPIdx(void* addr, HPLocal localData) {
	void** locHp = localData->localRecord->hp;
	for (int i = 0; i < localData->hpData->HP_COUNT; i++) {
		if (locHp[i] == addr) return i;
	}
	return -1;
}

//void prepareForReuse(void* data) {
//	//simple implementation - can be implemented with reuse
//	free(data);
//}

void SwapStacks(Stack* s1, Stack* s2) {
	Stack temp = *s1;
	*s1 = *s2;
	*s2 = temp;
}

}
