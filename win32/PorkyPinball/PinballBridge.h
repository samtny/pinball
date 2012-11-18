#pragma once

#include "PinballBridgeInterface.h"

class PinballBridge
{
public:
	PinballBridge(void);
	~PinballBridge(void);
	void init();

	const char * getPathForScriptFileName(void * scriptFileName);
	const char *getPathForTextureFilename(void * textureFilename);
	Texture *createRGBATexture(void *textureFilename);
    DisplayProperties *getDisplayProperties();
	void playSound(void * soundName);
	void addTimer(float duration, int timerId);

	ITimerDelegate *getTimerDelegate();
	void setTimerDelegate(ITimerDelegate *timerDelegate);

private:

	ITimerDelegate *_timerDelegate;

};

