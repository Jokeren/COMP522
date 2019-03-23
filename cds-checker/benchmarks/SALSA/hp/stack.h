/*
 * Stack
 *
 *      Author: Elad Gidron
 */

#ifndef STACK_
#define STACK_


Stack stackInit(int initialSize);

//Adds obj to stack. may resize the stack if needed.
void stackPush(Stack stack, void* addr, void* reclaimationFunc);

// removes head of stack and returns it.
// returns NULL if stack is empty.
ReclaimationData* stackPop(Stack stack);

// Returns the number of elements in the stack
int stackCount(Stack stack);

//resets the stack
void stackReset(Stack stack);

// Frees stacks' resources
void stackDestory(Stack stack);

#endif /* STACK_ */
