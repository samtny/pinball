
#include "PPPlayfield.h"
#include "PPPhysicsEngine.h"

#pragma once
class PPRenderer
{
public:
	PPRenderer(void);
	~PPRenderer(void);
	void init(PPPlayfield *playfield, PPPhysicsEngine *physicsEngine);
	void draw();
	PPPhysicsEngine *physicsEngine;
};

