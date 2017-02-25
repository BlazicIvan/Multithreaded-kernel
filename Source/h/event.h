#ifndef _event_h_
#define _event_h_

typedef unsigned char IVTNo;
class KernelEv;
#include "evImpl.h"

class Event {
public:
	Event (IVTNo ivtNo);
	~Event ();

	void wait ();

protected:
	friend class KernelEv;
	void signal(); // can call KernelEv
private:
	KernelEv* myImpl;
};

#define PREPAREENTRY(N,oldISR)\
	void interrupt ISR##N(...); \
	IVTEntry E##N(N, ISR##N); \
	void interrupt ISR##N(...)\
	{\
	E##N.signalEvent();\
	if (oldISR)\
		E##N.execOldISR();\
	}\


#endif
