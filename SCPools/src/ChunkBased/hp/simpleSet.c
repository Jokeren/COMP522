/*
 * simpleSet.c
 *
 * A very simple Set implementation.
 * The set is an array of constant size.
 * When an element is added it is appended to the current elements.
 * When setContains() is first called, the array is sorted, and then the element is found by binary search.
 *
 * This implementation is good when we first add all the elements and then perform all the searches.
 *
 *
 *      Author: Elad Gidron
 */

#include "simpleSet.h"
#include <stdlib.h>
#include <assert.h>

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

