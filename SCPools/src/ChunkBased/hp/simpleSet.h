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

#ifndef SIMPLESET_C_
#define SIMPLESET_C_

typedef struct set_t *Set, SetT;

// Returns a set with maximun size of maxSize.
Set setInit(int maxSize);
// Adds value val to set.
void setAdd(Set set, int addr);
// Returns TRUE if val is in set, otherwise returns false.
int setContains(Set set, int addr);
// Resets the set.
void setReset(Set set);
//Frees the Set's resources.
void setDestroy(Set set);



#endif /* SIMPLESET_C_ */
