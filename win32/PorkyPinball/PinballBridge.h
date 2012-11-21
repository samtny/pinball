#pragma once

#include "PinballBridgeInterface.h"

class PinballBridge
{
public:
	PinballBridge(void);
	~PinballBridge(void);
	void init();

	void setGameName(const char *gameName);
	const char * getPathForScriptFileName(void * scriptFileName);
	const char *getPathForTextureFilename(void * textureFilename);
	Texture *createRGBATexture(void *textureFilename);
    HostProperties *getHostProperties();
	void playSound(void * soundName);
	void addTimer(float duration, int timerId);

	ITimerDelegate *getTimerDelegate();
	void setTimerDelegate(ITimerDelegate *timerDelegate);

private:

	const char *_gameName;
	ITimerDelegate *_timerDelegate;

};

