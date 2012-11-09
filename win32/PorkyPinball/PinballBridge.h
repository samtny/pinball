#pragma once

#include "PinballBridgeInterface.h"

class PinballBridge
{
public:
	PinballBridge(void);
	~PinballBridge(void);
	void init();

	const char * getPathForScriptFileName(void * scriptFileName);
    DisplayProperties *getDisplayProperties();
	void playSound(void * soundName);
};

