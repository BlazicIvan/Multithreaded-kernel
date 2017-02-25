#include "semImpl.h"
#include "kernel.h"
#include "sleepLst.h"

/*
	Kernel's semaphore implementation.
*/

extern SleepList systemSleepList;

int KernelSem::wait(Time waitTime)
{

	syncLock
	if (--value < 0)
	{
		if (waitTime)
		{
			intd
			if (systemSleepList.add(waitTime, (KernelSem*)this) == -1)
			{
				inte
				return -1;
			}
			inte
		}
		signaled = 0;
		runningPCB->suspend();
		blocked.add((PCB*)runningPCB);
		dispatch();
	}
	syncUnlock
	return signaled;
}

void KernelSem::alert(PCB* toWake)
{
	signaled = 0;
	value++;
	toWake->wake();
	Thread* tmp = blocked.remove(toWake->getThreadID());
}

void KernelSem::signal()
{
	signaled = 1;
	if (value++ < 0)
	{
		PCB* temp = blocked.removeFirst();
		if(temp)
			temp->wake();
	}
}

KernelSem* newKernelSem(int value)
{
	if (value < 0)
	{
		genError("Initial semaphore value is invalid");
		return NULL;
	}

	syncLock
		KernelSem *newSem = new KernelSem(value);
	syncUnlock

	if (!newSem)
	{
		memError("KernelSem");
	}

	return newSem;
}
