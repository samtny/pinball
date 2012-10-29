
#include "PPPhysicsEngine.h"

#pragma once
class PPRenderer
{
public:
	PPRenderer(void);
	~PPRenderer(void);
	void init();
	void draw();
	PPPhysicsEngine *physicsEngine;
};

