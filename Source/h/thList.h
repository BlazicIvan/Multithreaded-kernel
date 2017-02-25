#ifndef _THLIST_H_
#define _THLIST_H_

#include "mydef.h"
#include "PCB.h"
#include "thread.h"

class Node;

//PCB* list
class ThreadList
{
private:

	class Node
	{
		friend class ThreadList;
	private:
		PCB *pcb;
		Node *next;
	public:

		Node(PCB *p)
		{
			pcb = p;
			next = NULL;
		}
		~Node()
		{
			pcb = NULL;
			next = NULL;
		}
		
	};

	Node *newNode(PCB *p);
	Node *head, *tail;
public:

	ThreadList();
	~ThreadList();

	//Wakes every thread in the list
	void wakeAll();

	//Adds the give PCB to the list
	void add(PCB *p);

	//Removes the first PCB in the list
	PCB *removeFirst();

	//Returns the matching ID thread and removes it
	Thread *remove(ID id);

	//Returns the matching ID thread
	Thread *search(ID key);
};

//Thread safe constructor
ThreadList* newThreadList();


#endif
