
#ifndef __ITIMERDELEGATE__
#define __ITIMERDELEGATE__

class ITimerDelegate {
public:
	virtual ~ITimerDelegate() {};
	virtual void timerCallback(int timerId) {};
};

#endif

