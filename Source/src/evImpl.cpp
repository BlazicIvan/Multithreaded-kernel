#include "evImpl.h"
#include "dos.h"
#include "kernel.h"

/*
	Kernel's event and IVTEntry implementation.
*/

IVTEntry* IVTEntry::entries[256] = {NULL};

IVTEntry::IVTEntry(IVTNo entryNo, ISR newISR)
{
	myEvent = NULL;
	myIVTNo = entryNo;
	oldISR = getvect(entryNo);
	if (entries[entryNo]==NULL)
	{
		entries[entryNo] = this;
		setvect(entryNo, newISR);
	}
	else
	{
		genError("IVT entry object already created");
	}
}

IVTEntry::~IVTEntry()
{
	setvect(myIVTNo, oldISR);
}

void IVTEntry::signalEvent()
{
	if (myEvent)
	{
		myEvent->signal();
	}
}

void IVTEntry::execOldISR()
{
	oldISR();
}

KernelEv::KernelEv(IVTNo entryNo)
{
	ready = false;
	owner = (PCB*)runningPCB;
	threadBlocked = false;
	myIVTNo = entryNo;
	if (IVTEntry::entries[entryNo])
	{
		if (IVTEntry::entries[entryNo]->myEvent == NULL)
			IVTEntry::entries[entryNo]->myEvent = this;
		else
			genError("IVT entry already assigned to another Event");
	}
	else
	{
		genError("IVT entry must be prepared for an Event first");
	}
}

KernelEv::~KernelEv()
{
	owner = NULL;
	IVTEntry::entries[myIVTNo]->myEvent = NULL;
}

void KernelEv::signal()
{
	if (!threadBlocked)
	{
		ready = true;
	}
	else
	{
		owner->wake();
		threadBlocked = false;
		dispatch();
	}
}

void KernelEv::wait()
{
	syncLock;
	if ((PCB*)runningPCB == owner)
	{
		if (!ready)
		{
			owner->suspend();
			threadBlocked = true;
			dispatch();
		}
		else
			ready = false;
	}
	syncUnlock;
}


KernelEv* newKernelEv(IVTNo entryNo)
{
	syncLock;
		KernelEv* newKE = new KernelEv(entryNo);
	syncUnlock;

	if(!newKE)
	{
		memError("KernelEv");
	}

	return newKE;
}
