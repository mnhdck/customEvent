#include "stdio.h"
#include "EventDefine.h"

#ifdef WIN32
#define PTO_ULONG(v) ((unsigned long)(unsigned __int64)v)
#else
#define PTO_ULONG(v) ((unsigned long)v)
#endif

class Sender
{
public:
	Event_Def(TestEvent, int);
};

class Reciever: public EventAccepter
{
public:
	void OnEvent(int param)
	{
		printf("test param=%d!\n", param);
	}
}

int main()
{
        Sender sender;
	Reciever rcv;
	
	ERegister(&sender, OnEvent, &rcv, &Reciever::OnEvent);
	ESendEvent(&sender, OnEvent, 100);
	EUnRegister(&sender, OnEvent, &rcv, &Reciever::OnEvent);
	return 0;
}
