
#ifndef __PINBALLNATIVE_C_INTERFACE_H__
#define __PINBALLNATIVE_C_INTERFACE_H__

#ifdef _WIN32
	#include "windows.h"
	#include "GL/gl.h"
#endif

typedef struct {
	int viewportX;
	int viewportY;
	int viewportHeight;
	int viewportWidth;
	float scale;
} DisplayProperties;

class PinballBridgeInterface
{
public:
    
	PinballBridgeInterface(void);
    ~PinballBridgeInterface(void);
    
#ifdef _WIN32
	void init(void);
#elif __APPLE__
	bool init(void);
#endif

	const char * getPathForScriptFileName(void * scriptFileName);

    DisplayProperties *getDisplayProperties();
	
	void playSound(void * soundName);

private:
#ifdef _WIN32
	void *_this;
#elif __APPLE__
    void *self;
#endif
};

#endif

