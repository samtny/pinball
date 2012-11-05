
#include "Playfield.h"
#include "Physics.h"

#pragma once
class Renderer
{
public:
	Renderer(void);
	~Renderer(void);
	void init(Playfield *playfield, Physics *physicsEngine);
	void draw();
	Physics *physicsEngine;
};

