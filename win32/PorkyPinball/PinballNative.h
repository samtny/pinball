#pragma once
#include "PinballNativeInterface.h"
class PinballNative
{
public:
	PinballNative(void);
	~PinballNative(void);
	void init();
	void initOpenGl();
	const char * getPathForScriptFileName(void * scriptFileName);
    DisplayProperties *getDisplayProperties();
	void playSound(void * soundName);
};

