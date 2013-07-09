
#ifndef __IPHYSICSDELEGATE__
#define __IPHYSICSDELEGATE__

class IPhysicsDelegate {
public:
	virtual ~IPhysicsDelegate() {};
	virtual void switchClosed(const char *switchName, const char *ballName) {};
	virtual void switchOpened(const char *switchName, const char *ballName) {};
};

#endif
