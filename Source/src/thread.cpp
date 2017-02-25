#include "thread.h"
#include "PCB.h"
#include "kernel.h"

/*
	Wrapper for PCB.
*/


Thread::Thread(StackSize stackSize, Time timeSlice)
{
	myPCB = newPCB(this, stackSize, timeSlice);
}

Thread::~Thread()
{
	PCB::allThreads->remove(myPCB->threadID);
	delete myPCB;
}

void Thread::start()
{
	myPCB->startThread();
}

ID Thread::getId()
{
	return myPCB->threadID;
}

ID Thread::getRunningId()
{
	return runningPCB->threadID;
}

Thread * Thread::getThreadById(ID id)
{
	if(id>=10)
		return PCB::allThreads->search(id);
	syncPrintf("Error: Forbiden thread ID!\n");
	return NULL;
}

void Thread::waitToComplete()
{
	if (myPCB->state != FINISHED && myPCB->state != NEW)
	{
		myPCB->addRunningToWL();
		dispatch();
	}
}

void dispatch()
{
	dispatchRequest = true;
	asm int 8h;
}
