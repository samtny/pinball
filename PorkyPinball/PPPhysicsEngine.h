#pragma once

#include "PPPlayfield.h"

class PPPhysicsEngine
{
public:
	PPPhysicsEngine(void);
	~PPPhysicsEngine(void);
	void init(PPPlayfield *playfield);
	void updatePhysics();
};

