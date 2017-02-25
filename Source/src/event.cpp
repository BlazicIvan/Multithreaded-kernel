#include "event.h"
#include "kernel.h"

/*
	Wrapper class for KernelEv.
*/

Event::Event(IVTNo ivtNo)
{
	if (ivtNo >= 0 && ivtNo < 256)
	{
		myImpl = newKernelEv(ivtNo);
	}
	else
	{
		genError("invalid IVT number for Event object");
	}
}

Event::~Event()
{
	delete myImpl;
}

void Event::signal()
{
	intd
	myImpl->signal();
	inte
}

void Event::wait()
{
	intd
	myImpl->wait();
	inte
}
