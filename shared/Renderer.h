
#include "Physics.h"

#pragma once
class Renderer
{
public:
	Renderer(void);
	~Renderer(void);
	void draw();
	Physics *physics;
};

