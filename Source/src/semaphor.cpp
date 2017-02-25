#include "semaphor.h"
#include "semImpl.h"

/*
	Wrapper class for KernelSem.
*/

Semaphore::Semaphore(int init)
{
	myImpl = newKernelSem(init);
}

Semaphore::~Semaphore()
{
	delete myImpl;
}

int Semaphore::wait(Time maxTimeToWait)
{
	return myImpl->wait(maxTimeToWait);
}

void Semaphore::signal()
{
	intd
	myImpl->signal();
	inte
}


int Semaphore::val() const
{
	int ret;
	syncLock;
	ret = myImpl->value;
	syncUnlock;
	return ret;
}
