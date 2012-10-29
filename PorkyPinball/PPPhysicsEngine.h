#pragma once

class PPPhysicsEngine
{
public:
	PPPhysicsEngine(void);
	~PPPhysicsEngine(void);
	void loadPlayfield(char *filename);
	void updatePhysics();
};

