/*
	MSQueue is a template-based implementation of Michael&Scott lock-free queue
*/
#ifndef MSQUEUE_H_
#define MSQUEUE_H_
//#include "Statistics.h"
#include "Atomic.h"
#include <iostream>
using namespace std;

template<class T> class MSQueue{

	int queueSize;
	struct node_t;

	/* 
		we use a union to implement pointer_t so atomic double-width CAS operations will be possible
		(such operations are allowed only on integral type with 2 words width - see documentation of X86 cmpxchg16b and cmpxchg8b instructions)
	*/
	union pointer_t{
		struct {
			node_t* ptr;		// node pointer
			unsigned long count;	// modifications counter
		}p;
		DWORD dw;
		
		//constructors of pointer_t union
		pointer_t(){
			this->p.ptr = NULL;
			this->p.count = 0;
		}
		pointer_t(node_t* pNode, const unsigned long c){
			this->p.ptr = pNode;
			this->p.count = c;
		}
		pointer_t(const pointer_t& point){
			this->p.ptr = point.p.ptr;
			this->p.count = point.p.count;
		}			
		
		/* overloaded operators */
		bool operator == (pointer_t point){return (this->p.ptr == point.p.ptr)&&(this->p.count == point.p.count);}
		void operator = (pointer_t point){this->p.ptr = point.p.ptr; this->p.count = point.p.count;}
	};
	
	struct node_t{
		T value;
		pointer_t next;
	};
	
	/* queue's head and tail */
	pointer_t Head;
	pointer_t Tail;
	
	
	/* performs double-width CAS of pointer_t objects and updates CAS statistics in stat */
	bool CAS(pointer_t& dest, pointer_t& oldVal, pointer_t& newVal/*, Statistics* stat*/){    
		char casRes = 0;
		casRes = DWCAS(&(dest.dw),oldVal.dw,newVal.dw);
		//stat->CAS_count_inc();
		bool retVal = (casRes == 0) ? false : true;
		//if(!retVal){stat->CAS_failures_inc();}
		return retVal; 
	}
	

public:
	/* debug function for printing uint128 */
	void print128uint(DWORD dw){
		printf("%lu%lu", dw, dw);
	}
		
	/* constructor - allocate a free node - only node in the list */
	MSQueue(){
		node_t* pNode = new node_t();
		pNode->next.p.ptr = NULL;
		Head.p.ptr = Tail.p.ptr = pNode;
		queueSize = 0;
	}
	
	/* destructor - delete dummy head */
	~MSQueue(){
		delete Head.p.ptr; 
	}
	
	int getSize(){
		return queueSize;
	}

	void enqueue(const T& t/*, Statistics* stat*/){
		node_t* pNode = new node_t();  // allocate a free node 
		pNode->value = t;
		pNode->next.p.ptr = NULL;
		
		pointer_t tail;
		while(true)   // keep trying until enqueue is done
		{
			tail = Tail;  // "read Tail.ptr and Tail.count together" - do we need an "atomic" assignments / copy constructor ?
			pointer_t next(tail.p.ptr->next);  // read next ptr and count together
			if(tail == Tail)   // are tail and next consistent? - default field-by-fields comparison
			{
				if(next.p.ptr == NULL)  // was Tail pointing to the last node?
				{
					pointer_t updated_next(pNode,next.p.count+1);		
					if(CAS(tail.p.ptr->next,next,updated_next/*,stat*/))  // try to link node at the end of the list
					{
						__sync_fetch_and_add(&queueSize,1);
						break;  //enqueue is done - exit loop
					}
				}
				else  // Tail wasn't pointing to the last node
				{
					pointer_t updated_tail(next.p.ptr,tail.p.count+1);	
					CAS(Tail,tail,updated_tail/*,stat*/);  // try to swing Tail to the next node
				}
			}
		}
		pointer_t updated_tail(pNode,tail.p.count+1);
		CAS(Tail,tail,updated_tail/*,stat*/);  //enqueue is done - try to swing Tail to the inserted node
	}

	bool dequeue(T& pt/*, Statistics* stat*/){
		pointer_t head;
		while(true)		// keep trying until dequeue is done
		{
			head = Head;  // read Head
			pointer_t tail = Tail;  // read Tail
			if(head.p.ptr == NULL)
			{
				return false;
			}
			pointer_t next = head.p.ptr->next;  // read Head.ptr->next
			if(head == Head)  // are head,tail and next consistent
			{
				if(head.p.ptr == tail.p.ptr)  //is the queue empty or Tail is falling behind
				{
					if(next.p.ptr == NULL)  // is the queue empty?
					{					
						return false;
					}
					pointer_t updated_tail(next.p.ptr,tail.p.count+1);
					CAS(Tail,tail,updated_tail/*,stat*/);  // Tail is falling behind. Try to advance it
				}
				else  // no need to deal with Tail
				{				
					pt = next.p.ptr->value;  //read value before CAS. otherwise another dequeue might free the next node
					pointer_t updated_head(next.p.ptr,head.p.count+1);
					if(CAS(Head,head,updated_head/*,stat*/))  // try to swing Head to the next node
					{
						__sync_fetch_and_sub(&queueSize,1);
						break;  //dequeue is done - exit node
					}
				}
			}
		}
		delete head.p.ptr; // it is now safe to free old dummy node. t itself will be freed outside by the caller 
		return true;
	}
};

#endif /* MSQUEUE_H_ */
