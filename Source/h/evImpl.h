#ifndef _EVIMPL_H_
#define _EVIMPL_H_

#include "event.h"
#include "mydef.h"
#include "PCB.h"

typedef void interrupt(*ISR)(...);

//Kernel event
class KernelEv
{
private:

	bool ready,threadBlocked;
	PCB* owner;
	IVTNo myIVTNo;

public:
	KernelEv(IVTNo entryNo);
	~KernelEv();

	//Wait for event
	void wait();

	//Signal from ISR
	void signal();
	
};

//Thread-safe constructor
KernelEv* newKernelEv(IVTNo entryNo);

//IVT entry control
class IVTEntry
{
private:
	friend class KernelEv;
	friend class Event;

	KernelEv *myEvent;
	ISR oldISR;
	IVTNo myIVTNo;

	static IVTEntry*  entries[256];

public:

	IVTEntry(IVTNo entryNo, ISR newISR);
	~IVTEntry();

	//Send signal to event object
	void signalEvent();

	//Execute original ISR
	void execOldISR();
};


#endif
