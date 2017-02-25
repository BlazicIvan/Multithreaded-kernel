#include "PCB.h"
#include <DOS.H>
#include "kernel.h"
#include "sleepLst.h"

/*
	Kernel's PCB implementation.
*/

//List of all user threads
ThreadList* PCB::allThreads;

//First ID for system threads
ID PCB::nextID = 1;

//System idle thread
PCB *idleThreadPCB;

//User thread
PCB *userThreadPCB;

//Running thread
volatile PCB *runningPCB=NULL;

//Remaining execution time
volatile Time remainingTime;

//List of all timer requests
SleepList systemSleepList;

//Dispatch requested
volatile char dispatchRequest = 0;

//Dispatch enabled
volatile char dispatchEnable = 1;

//System threads initialized
bool pcbInitialized = false;

unsigned tmpSS, tmpSP, tmpBP;

//Kernel main context
unsigned retSS, retSP, retBP;



PCB::PCB(Thread *linkedThread, StackSize stackSize, Time timeSlice)
{
	myThread = linkedThread;
	threadStackSize = stackSize;
	threadTimeSlice = timeSlice;
	WaitList = newThreadList();
	blockedCount = 0;

	//System threads are not stored in user threads list
	if (pcbInitialized)
	{
		PCB::allThreads->add(this);
	}

	threadID = PCB::nextID++;

	stack = new unsigned[stackSize];

	if (stack)
	{
		stack[stackSize - 5] = 0x200;					//PSWI = 1
		stack[stackSize - 6] = FP_SEG(threadFunction);	//Thread function segment
		stack[stackSize - 7] = FP_OFF(threadFunction);	//Thread function offset

		//Reserve space for registers
		SP = FP_OFF(stack + stackSize - 16);
		SS = FP_SEG(stack + stackSize - 16);
		BP = SP;
	}

	state = NEW;
}



PCB *newPCB(Thread *linkedThread, StackSize stackSize, Time timeSlice)
{
	if (stackSize > MAX_STACKSIZE)
	{
		genError("Error: maximum stack size exceeded!\n");
		return NULL;
	}

	syncLock
		PCB *n = new PCB(linkedThread, stackSize, timeSlice);
	syncUnlock

	if (n==NULL)
	{
		memError("PCB");
	}
	return n;
}


void PCB::startThread()
{
	if (state == NEW)
	{
		syncLock
		state = READY;
		Scheduler::put(this);
		syncUnlock
	}
}

PCB::~PCB()
{
	myThread = NULL;
	delete stack;
}

void PCB::addRunningToWL()
{
	syncLock
	runningPCB->suspend();
	WaitList->add((PCB*)runningPCB);
	syncUnlock
}

void PCB::threadFunction()
{
	runningPCB->myThread->run();

	syncLock
	runningPCB->WaitList->wakeAll();
	runningPCB->state = FINISHED;
	syncUnlock

	dispatch();
}

Thread* PCB::getThread()
{
	return myThread;
}

ID PCB::getThreadID()
{
	return threadID;
}

void PCB::wake()
{
	if(state != READY && state != FINISHED)
	{
		blockedCount--;
		if(blockedCount==0)
		{
			state = READY;
			Scheduler::put(this);
		}
	}
}

void PCB::suspend() volatile
{
	state = BLOCKED;
	blockedCount++;
}

void IdleThread::run()
{
	while(1);
}

void PCB::initialize(UserThread *userThread, IdleThread *idle)
{
	/*
		Note: User threads are created after system initialization.
	*/

	allThreads = newThreadList();

	idle->myPCB->state = SYSTEM;
	idleThreadPCB = idle->myPCB;

	userThreadPCB = userThread->myPCB;
	runningPCB = userThreadPCB;

	pcbInitialized = true;

	nextID=10;
}

void UserThread::run()
{
	result = userMain(argc_,argv_);
	exitSystem();
}

void interrupt PCB::runUserThread()
{

	//Save the return address in main
	asm{
		mov retSP, sp
		mov retSS, ss
		mov retBP, bp
		}


	userThreadPCB->state = READY;

	//Set up the stack for userThread
	tmpBP = userThreadPCB->BP;
	tmpSP = userThreadPCB->SP;
	tmpSS = userThreadPCB->SS;

	asm{
		mov sp, tmpSP
		mov ss, tmpSS
		mov bp, tmpBP
	}
}

void interrupt exitSystem()
{
	//Return to kernel
	asm{
			mov sp, retSP
			mov ss, retSS
			mov bp, retBP
		}

}


void interrupt timerISR(...)
{
	if (!dispatchRequest)
	{
		//Time update
		asm int 60h;
		systemSleepList.updateTime();
		tick();
		remainingTime--;
	}

	if ((remainingTime == 0 && runningPCB->threadTimeSlice) || dispatchRequest)
	{
		if (dispatchEnable)
		{
			//Context switch
			asm{
				mov tmpSP, sp
				mov tmpSS, ss
				mov tmpBP, bp
			}

			runningPCB->BP = tmpBP;
			runningPCB->SP = tmpSP;
			runningPCB->SS = tmpSS;


			if (runningPCB->state == EXEC)
			{
				runningPCB->state = READY;
				Scheduler::put((PCB*)runningPCB);
			}

			runningPCB = Scheduler::get();

			if (runningPCB == NULL)
			{
				runningPCB = idleThreadPCB;
			}
			else
				runningPCB->state = EXEC;

			remainingTime = runningPCB->threadTimeSlice;


			tmpBP = runningPCB->BP;
			tmpSP = runningPCB->SP;
			tmpSS = runningPCB->SS;

			asm{
				mov sp, tmpSP
				mov ss, tmpSS
				mov bp, tmpBP
			}

			dispatchRequest = false;
		}
		else
			dispatchRequest = true;
	}

}

