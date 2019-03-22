/*
	MSQueue is a template-based implementation of Michael&Scott lock-free queue
*/
#ifndef MSQUEUE_H_
#define MSQUEUE_H_
#include "AtomicStatistics.h"
#include "Atomic.h"
#include <iostream>
using namespace std;

template<class T> class MSQueue{

  std::atomic<int> queueSize;
	
	struct node_t {
		T value;
    AtomicWrapper<node_t *> next;
	};
	
	/* queue's head and tail */
  AtomicWrapper<node_t *> Head;
  AtomicWrapper<node_t *> Tail;
	
	
	/* performs double-width CAS of pointer_t objects and updates CAS statistics in stat */
	bool mCAS(AtomicWrapper<node_t *> &dest, node_t *oldVal, node_t *newVal, AtomicStatistics* stat){
    bool retVal = dest.compare_exchange_strong(oldVal, newVal);
		stat->CAS_count_inc();
		if(!retVal){stat->CAS_failures_inc();}
		return retVal; 
	}
	

public:
	/* constructor - allocate a free node - only node in the list */
	MSQueue(){
		node_t* pNode = new node_t();
    pNode->next.store(NULL);
		Head.store(pNode);
    Tail.store(pNode);
    queueSize.store(0);
	}
	
	/* destructor - delete dummy head */
	~MSQueue(){
		delete Head.load();
	}
	
	int getSize(){
		return queueSize.load();
	}

	void enqueue(const T& t, AtomicStatistics* stat){
		node_t* pNode = new node_t();  // allocate a free node 
		pNode->value = t;
    pNode->next.store(NULL);
		
		node_t *tail;
		while(true)   // keep trying until enqueue is done
		{
			tail = Tail.load();  // "read Tail.ptr and Tail.count together" - do we need an "atomic" assignments / copy constructor ?
			node_t *next = tail->next.load();  // read next ptr and count together
			if(tail == Tail.load())   // are tail and next consistent? - default field-by-fields comparison
			{
				if(next == NULL)  // was Tail pointing to the last node?
				{
					node_t *updated_next = pNode;
					if(mCAS(tail->next,next,updated_next,stat))  // try to link node at the end of the list
					{
						FAA(&queueSize, 1);
						stat->FetchAndIncCount_inc();
						break;  //enqueue is done - exit loop
					}
				}
				else  // Tail wasn't pointing to the last node
				{
					node_t *updated_tail = next;
					mCAS(Tail,tail,updated_tail,stat);  // try to swing Tail to the next node
				}
			}
		}
		node_t *updated_tail = pNode;
		mCAS(Tail,tail,updated_tail,stat);  //enqueue is done - try to swing Tail to the inserted node
	}

	bool dequeue(T& pt, AtomicStatistics* stat){
		node_t *head;
		while(true)		// keep trying until dequeue is done
		{
			head = Head.load();  // read Head
			node_t *tail = Tail.load();  // read Tail
			if(head == NULL)
			{
				return false;
			}
			node_t *next = head->next.load();  // read Head.ptr->next
			if(head == Head.load())  // are head,tail and next consistent
			{
				if(head == tail)  //is the queue empty or Tail is falling behind
				{
					if(next == NULL)  // is the queue empty?
					{					
						return false;
					}
					node_t *updated_tail = next;
					mCAS(Tail,tail,updated_tail,stat);  // Tail is falling behind. Try to advance it
				}
				else  // no need to deal with Tail
				{
					if (next == NULL) {
						continue; // something strange happened :)
					}
					pt = next->value;  //read value before CAS. otherwise another dequeue might free the next node
					node_t *updated_head = next;
					if(mCAS(Head,head,updated_head,stat))  // try to swing Head to the next node
					{
            queueSize.fetch_add(1);
						stat->FetchAndIncCount_inc();
						break;  //dequeue is done - exit node
					}
				}
			}
		}
		delete head; // it is now safe to free old dummy node. t itself will be freed outside by the caller 
		return true;
	}
};

#endif /* MSQUEUE_H_ */
