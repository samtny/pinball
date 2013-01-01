
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
	float overlayScale;
} HostProperties;

typedef struct {
	int bpp;
	int width;
	int height;
	void *data;
} GLTexture;

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

	void setGameName(const char *gameName);

	const char *getGameName();

	const char *getPathForScriptFileName(void *scriptFileName);

	const char *getPathForTextureFileName(void *textureFileName);

	GLTexture *createRGBATexture(void *textureFileName);

    HostProperties *getHostProperties();
	
	void playSound(void *soundName);

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

