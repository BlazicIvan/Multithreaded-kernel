#include "semaphor.h"
#include "PCB.h"


//Timed list
class SleepList
{
private:
	class SleepNode
	{
		friend class SleepList;
	private:
		PCB *pcb;
		Time sleepTime;
		KernelSem *toAlert;
		SleepNode *next;
	public:

		SleepNode(PCB *p, Time time, KernelSem *s)
		{
			pcb = p;
			sleepTime = time;
			toAlert = s;
			next = NULL;
		}
		~SleepNode()
		{
			pcb = NULL;
			toAlert = NULL;
			next = NULL;
		}
	};

	SleepNode *head;
	SleepNode *newSleepNode(PCB *p, Time time, KernelSem *s);
public:

	SleepList() 
	{
		head = NULL;
	};
	~SleepList();

	//Add a timer request
	int add(Time sleepTime, KernelSem *toAlert);
	
	//Update list time
	void updateTime();

};
