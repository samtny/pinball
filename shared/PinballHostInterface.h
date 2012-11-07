
// TODO: use this same header for multi-platform via compiler flags?

#ifndef __PINBALLHOST_C_INTERFACE_H__
#define __PINBALLHOST_C_INTERFACE_H__

typedef struct {
	int viewHeight;
	int viewWidth;
} PinballHostEnvironment;

class PinballHostImpl
{
public:
    
	PinballHostImpl(void);
    ~PinballHostImpl(void);
    
	const char * getPathForScriptFileName(void * scriptFileName);
    PinballHostEnvironment *getPinballHostEnvironment();
	
	void playSound(void * soundName);

private:
    void *self;
};

#endif

