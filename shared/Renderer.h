
#include "Physics.h"

#pragma once

class PinballNativeImpl;

class Renderer
{
public:
	Renderer(void);
	~Renderer(void);
	void init(PinballNativeImpl *pinballNative);
	void draw();
	Physics *physics;
private:
	PinballNativeImpl *_pinballNative;
};

