#include "PPPhysicsEngine.h"
#include "chipmunk.h"

static cpSpace *space;
static cpVect gravity = cpv(0.0, 9.80665f);

static cpFloat timeStep = 1.0/180.0;

PPPhysicsEngine::PPPhysicsEngine(void)
{

	space = cpSpaceNew();
	cpSpaceSetGravity(space, gravity);

}


PPPhysicsEngine::~PPPhysicsEngine(void)
{
	cpSpaceFree(space);
}

void PPPhysicsEngine::loadPlayfield(char *filename) {
	
	

}

void PPPhysicsEngine::updatePhysics() {

	cpSpaceStep(space, timeStep);

}