#pragma once

#include "PinballNativeInterface.h"

#ifdef _WIN32

class PinballHost
{
public:
	PinballHost(void);
	~PinballHost(void);
	void start(const char *gameName);
private:
	bool finished;
	bool paused;
};

#endif