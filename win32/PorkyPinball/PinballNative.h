#pragma once
#include "PinballNativeInterface.h"
class PinballNative
{
public:
	PinballNative(void);
	~PinballNative(void);
	const char * getPathForScriptFileName(void * scriptFileName);
    DisplayProperties *getDisplayProperties();
	void playSound(void * soundName);
};

