/*
 * stack.c
 *
 * Very simple adjustable stack.
 *
 *      Author: Elad Gidron
 */

#include "stack.h"
#include <stdlib.h>
#include <assert.h>

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
void stackPush(Stack stack, void* addr, ReclaimationFunc reclaimationFunc) {
	//resize if needed
	if (stack->rcount == stack->size - 1) {
		stack->data = realloc(stack->data,sizeof(ReclaimationData) * stack->size*2);
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
