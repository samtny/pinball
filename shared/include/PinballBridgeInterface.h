
#ifndef __PINBALLNATIVE_C_INTERFACE_H__
#define __PINBALLNATIVE_C_INTERFACE_H__

#include "TimerDelegate.h"

#ifdef _WIN32
	#include "windows.h"
	#include "GL/gl.h"
#endif

typedef struct HostProperties {
	int viewportX;
	int viewportY;
	int viewportHeight;
	int viewportWidth;
    float fontScale;
	float overlayScale;
} HostProperties;

typedef struct GLTexture {
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
    
	void setGameName(const char *gameName);
	const char *getGameName();
	const char *getBasePath();
	const char *getScriptPath(const char *scriptName);
	const char *getTexturePath(const char *textureName);
    	const HostProperties *getHostProperties();
	GLTexture *createRGBATexture(void *textureName);
	void playSound(const char *soundName);
	void addTimer(float duration, int id, const ITimerDelegate *timerDelegate);

private:
#ifdef __APPLE__
	void *self;
#else
	void *_this;
#endif
	ITimerDelegate *_timerDelegate;
};

#endif

