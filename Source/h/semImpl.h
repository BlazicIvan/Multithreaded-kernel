#ifndef _KERNELSEM_H_
#define _KERNELSEM_H_

#include "PCB.h"
#include "semaphor.h"
#include "thList.h"


class KernelSem
{
private:

	friend class Semaphore;

	int value;
	volatile int signaled;
	ThreadList blocked;


public:

	KernelSem(int initValue) : value(initValue), signaled(1){};

	//Wait on semaphore
	int wait(Time waitTime);

	//Signal on semaphore
	void signal();

	//Signal if time is out
	void alert(PCB* toWake);

};

//Thread-safe constructor wrapper
KernelSem *newKernelSem(int value);

#endif
