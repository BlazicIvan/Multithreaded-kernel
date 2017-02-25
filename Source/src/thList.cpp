#include "thList.h"
#include "kernel.h"

/*
	Implementation of ThreadList as standard lists.
*/

ThreadList::ThreadList()
{
	head = tail = NULL;
}

ThreadList::~ThreadList()
{
	Node *n = head;
	Node *prev;
	while (n)
	{
		prev = n;
		n = n->next;
		delete prev;
	}
	head = tail = NULL;
}

void ThreadList::add(PCB *p)
{
	Node *newN = newNode(p);
	if (newN)
	{
		if (head == NULL)
		{
			head = tail = newN;
		}
		else
		{
			tail->next = newN;
			tail = newN;
		}
	}
}

PCB* ThreadList::removeFirst()
{
	if (head == NULL)
		return NULL;
	PCB *ret = head->pcb;
	Node *prev = head;
	if(tail == head)
		tail = tail->next;
	head = head->next;
	delete prev;
	return ret;
}



Thread* ThreadList::remove(ID id)
{
	Node *prev, *cur;
	prev = NULL;
	cur = head;
	while (cur && cur->pcb->threadID != id)
	{
		prev = cur;
		cur = cur->next;
	}
	if (cur == NULL)
		return NULL;
	Thread *found = cur->pcb->myThread;
	if(prev)
		prev->next = cur->next;
	if (cur == head)
		head = head->next;
	if(cur==tail)
		tail = prev;
	delete cur;
	return found;
}

Thread* ThreadList::search(ID id)
{
	Node *cur = head;
	while (cur && cur->pcb->threadID != id)
		cur = cur->next;
	if (cur == NULL)
		return NULL;
	return cur->pcb->myThread;
}

ThreadList* newThreadList()
{
	syncLock
		ThreadList *newPtr = new ThreadList();
	syncUnlock
	if (newPtr == NULL)
		memError("ThreadList");
	return newPtr;
}

ThreadList::Node* ThreadList::newNode(PCB *p)
{
	syncLock
		Node *newPtr = new Node(p);
	syncUnlock
	if (newPtr == NULL)
		memError("ThreadList::Node");
	return newPtr;
}

void ThreadList::wakeAll()
{
	Node *n = head;
	Node *prev;
	while (n)
	{
		n->pcb->wake();
		prev = n;
		n = n->next;
		delete prev;
	}
	head = tail = NULL;
}

