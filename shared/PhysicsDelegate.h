
#ifndef __IPHYSICSDELEGATE__
#define __IPHYSICSDELEGATE__

class IPhysicsDelegate {
public:
	virtual ~IPhysicsDelegate() {};
	virtual void switchClosed(const char *switchName) {};
	virtual void switchOpened(const char *switchName) {};
};

#endif