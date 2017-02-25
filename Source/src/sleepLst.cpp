#include "sleepLst.h"
#include "mydef.h"
#include "semImpl.h"
#include "kernel.h"

/*
	Implementation of timed lists.
*/

SleepList::~SleepList()
{
	head = NULL;
}

void SleepList::updateTime()
{
	SleepNode *prev;
	if (!head)
		return;
	head->sleepTime--;
	while (head && head->sleepTime == 0)
	{
		head->toAlert->alert(head->pcb);
		prev = head;
		head = head->next;
		delete prev;
	}
}

SleepList::SleepNode* SleepList::newSleepNode(PCB *p, Time time, KernelSem *s)
{
	syncLock
		SleepNode *newPtr = new SleepNode(p, time, s);
	syncUnlock
	if (newPtr == NULL)
		memError("SleepList::SleepNode");
	return newPtr;
}


int SleepList::add(Time sleepTime, KernelSem *toAlert)
{
	if (sleepTime > 0)
	{
		SleepNode *added = newSleepNode((PCB*)runningPCB, sleepTime, toAlert);
		SleepNode *cur = head, *prev = NULL;
		if (!cur)
		{
			head = added;
			return 0;
		}
		while (cur && sleepTime > 0)
		{
			if (sleepTime > cur->sleepTime)
			{
				sleepTime -= cur->sleepTime;
			}
			else
			{
				if (prev)
				{
					prev->next = added;
				}
				else
				{
					head = added;
				}
				added->next = cur;
				cur->sleepTime -= sleepTime;
				added->sleepTime = sleepTime;
				return 0;
			}
			prev = cur;
			cur = cur->next;
		}
		added->sleepTime = sleepTime;
		prev->next = added;
		return 0;
	}
	else
	{
		genError("Sleep time must be greater than zero");
		return -1;
	}
}
