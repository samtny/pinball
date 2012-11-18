
#ifndef __PINBALLNATIVE_C_INTERFACE_H__
#define __PINBALLNATIVE_C_INTERFACE_H__

#include "TimerDelegate.h"

#ifdef _WIN32
	#include "windows.h"
	#include "GL/gl.h"
#endif

typedef struct {
	int viewportX;
	int viewportY;
	int viewportHeight;
	int viewportWidth;
    float fontScale;
} DisplayProperties;

typedef struct {
	int bpp;
	int width;
	int height;
	void *data;
} Texture;

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

	const char *getPathForTextureFileName(void *textureFileName);

	Texture *createRGBATexture(void *textureFileName);

    DisplayProperties *getDisplayProperties();
	
	void playSound(void * soundName);

	void setTimerDelegate(ITimerDelegate *timerDelegate);

	void addTimer(float duration, int id);

private:
#ifdef _WIN32
	void *_this;
#elif __APPLE__
    void *self;
#endif
	ITimerDelegate *_timerDelegate;
};

#endif

