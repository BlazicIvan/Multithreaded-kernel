#include <DOS.H>
#include <STDIO.H>
#include <STDARG.H>
#include "kernel.h"
#include "PCB.h"

/*
	Kernel's main code.
*/

void interrupt (*oldISR)(...);


int syncPrintf(const char *format, ...)
{
	int res;
	va_list args;
	syncLock
		va_start(args, format);
	res = vprintf(format, args);
	va_end(args);
	syncUnlock
		return res;
}


void memError(const char *msg)
{
	syncPrintf("Error: memory allocation for %s failed!\n", msg);
	exitSystem();
}


void genError(const char *msg)
{
	syncPrintf("Error: %s!\n", msg);
}

//Set new timer ISR
void set()
{
	asm cli;
	oldISR = getvect(0x08);
	setvect(0x60,oldISR);
	setvect(0x08, timerISR);
	asm sti;
}


//Restore original timer ISR
void restore()
{
	asm cli;
	setvect(0x08,oldISR);
	asm sti;
}


//Kernel code
int main(int argc, char *argv[])
{
	int result;
	IdleThread *idle;
	UserThread *userMainThread;
	userMainThread = new UserThread(argc, argv);
	idle = new IdleThread();

	PCB::initialize(userMainThread, idle);

	set();
	PCB::runUserThread();
	restore();

	result = userMainThread->getResult();

	delete idle;
	delete userMainThread;

	return result;
}

