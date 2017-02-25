//Thread system implementation
#ifndef _PCB_H_
#define _PCB_H_

#include "mydef.h"
#include "SCHEDULE.H"
#include "thList.h"
#include "thread.h"

typedef char threadState;

//Thread states
#define NEW 0
#define READY 1
#define EXEC 2
#define BLOCKED 3
#define FINISHED 4

//System threads
#define SYSTEM 5

#define MAX_STACKSIZE 65536/sizeof(unsigned) //64KB limit

//Synchronization variables
extern volatile char dispatchRequest;
extern volatile char dispatchEnable;

//Synchronization macros
#define syncLock 	dispatchEnable=0;\

#define syncUnlock 	dispatchEnable=1;\
					if (dispatchRequest)\
						dispatch();\

//Disable interrupts
#define intd asm pushf; asm cli;

//Enable interrupts
#define inte asm popf;

extern volatile PCB* runningPCB;

class ThreadList;

//System idle thread
class IdleThread : public Thread
{
private:
	friend class PCB;
public:

	IdleThread() : Thread(16, 1) {};
protected:
	virtual void run();
};


//User thread
class UserThread : public Thread
{
private:
	friend class PCB;
	int argc_, result;
	char** argv_;

public:
	UserThread(int argc, char** argv) : Thread()
	{
		argc_ = argc;
		argv_ = argv;
		result = 0;
	};

	int getResult()
	{
		return result;
	}

protected:
	virtual void run();
};

//Process control block
class PCB
{
	friend class Thread;
	friend class ThreadList;
	friend class KernelSem;
	friend void interrupt timerISR(...);
private:

	//Thread identificator
	static ID nextID;
	ID threadID;

	//Execution context
	unsigned SS, SP, BP;
	unsigned *stack;
	threadState state;
	StackSize threadStackSize;

	//Dedicated time slice
	Time threadTimeSlice;

	//List of threads that wait for current to finish
	ThreadList *WaitList;

	//Linked Thread object
	Thread *myThread;

	//Number of "blocked" lists this thread is in
	unsigned blockedCount;

public:

	PCB(Thread *linkedThread, StackSize stackSize, Time timeSlice);
	~PCB();

	//Must be called from synchronized blocks!
	void wake();
	void suspend() volatile;

	//Activates Thread object
	void startThread();

	//Adds running thread to waiting list
	void addRunningToWL();

	//Linked Thread ID
	ID getThreadID();

	//Returns linked Thread object
	Thread *getThread();

	//List of all user threads
	static ThreadList *allThreads;

	//run() wrapper
	static void threadFunction();

	//Runs the userMain thread
	static void interrupt runUserThread();

	//Must be called before using PCB
	static void initialize(UserThread *userThread, IdleThread *idle);
	
};


//Thread safe PCB constructor
PCB *newPCB(Thread *linkedThread, StackSize stackSize, Time timeSlice);

//Custom timer ISR
void interrupt timerISR(...);

//Returns from userMain to kernel
void interrupt exitSystem();




#endif
