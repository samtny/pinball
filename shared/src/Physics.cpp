
#include "Physics.h"

#include "PinballBridgeInterface.h"

#include "PhysicsDelegate.h"

#include "Playfield.h"

#include "Parts.h"

#include "Util.h"

#include "chipmunk/chipmunk.h"

#include <iostream>

extern "C" {
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}

using std::string;
using std::map;

static Physics *physics_currentInstance;

//static cpSpace *_space;

static cpVect gravity = cpv(0.0, 9.80665f);

static double _boxStiffness = 200.0;
static double _boxDamping = 100.0;

static double flipImpulse = 0.02f;
static double unflipImpulse = 0.02f;

static double flipForce = 0.2f;
static double unflipForce = 0.2f;

static double _nudgeImpulse = 1.0;

static double timeStep = 1.0/180.0;

static int iterations = 10;

static float scale = 37;

Physics::Physics(void)
{
	physics_currentInstance = this;
	_paused = false;
}

void Physics::setBridgeInterface(PinballBridgeInterface *bridgeInterface) {
	_bridgeInterface = bridgeInterface;
}

void Physics::setDelegate(IPhysicsDelegate *delegate) {
	_delegate = delegate;
}

IPhysicsDelegate * Physics::getDelegate() {
	return _delegate;
}

void Physics::setPlayfield(Playfield *playfield) {
	_playfield = playfield;
}

void Physics::init() {

	this->loadConfig();
    this->loadForces();

	_space = cpSpaceNew();
	cpSpaceSetIterations(_space, iterations);
		
	this->createObjects();
	
	this->initCollisionHandlers();

	cpSpaceSetGravity(_space, gravity);

}

static int __ballPreSolve(cpArbiter *arb, cpSpace *space, void *unused) {

	return physics_currentInstance->ballPreSolve(arb, space, unused);

}

int Physics::ballPreSolve(cpArbiter *arb, cpSpace *space, void *unused) {

	// TODO: move to local var;
	map<string, Material> mats = *_playfield->getMaterials();

	Material mat = mats["steel"];

	arb->e = mat.e;
	arb->u = mat.f;

	return 1;

}

cpSpace *Physics::getSpace() {
	return _space;
}

float Physics::getScale() {
	return scale;
}

static void destroyConstraint(cpBody *body, cpConstraint *constraint, void *data) {
	cpSpace *space = cpBodyGetSpace(body);
	cpSpaceRemoveConstraint(space, constraint);
	cpConstraintFree(constraint);
}

static void destroyShape(cpBody *body, cpShape *shape, void *data) {
	cpSpace *space = cpBodyGetSpace(body);
	cpSpaceRemoveShape(space, shape);
	cpShapeFree(shape);
}

static void destroyObject(cpSpace *space, void *itm, void *unused) {
	// TODO: yep
	/*
	LayoutItem *item = (LayoutItem *)itm;

	LayoutItem box = physics_currentInstance->getLayoutItems()->find("box")->second;

	if (item->body && (strcmp(item->n.c_str(), "box") == 0 || item->body != box.body)) {
		cpBodyEachConstraint(item->body, destroyConstraint, NULL);
		cpBodyEachShape(item->body, destroyShape, NULL);
		cpSpaceRemoveBody(space, item->body);
		cpBodyFree(item->body);
	} else if (item->shape) {
		cpSpaceRemoveShape(space, item->shape);
		cpShapeFree(item->shape);
	}
	*/
	LayoutItem *item = (LayoutItem *)itm;

	for (int i = 0; i < (int)item->bodies.size(); i++) {
		cpBody *body = item->bodies[i];
		cpBodyEachConstraint(body, destroyConstraint, NULL);
		cpBodyEachShape(body, destroyShape, NULL);
		cpSpaceRemoveBody(space, body);
		cpBodyFree(body);
	}
	item->bodies.clear();

	for (int i = 0; i < (int)item->shapes.size(); i++) {
		cpShape *shape = item->shapes[i];
		cpSpaceRemoveShape(space, shape);
		cpShapeFree(shape);
	}
	item->shapes.clear();

}

void Physics::destroyObject(LayoutItem *item) {

	if (cpSpaceIsLocked(_space)) {
		cpSpaceAddPostStepCallback(_space, ::destroyObject, item, NULL);
	} else {
		::destroyObject(_space, item, NULL);
	}

}

void Physics::createObjects(void) {

	LayoutItem *box = &_playfield->getLayout()->find("box")->second;
	this->createObject(box);

	for (it_LayoutItem iterator = _playfield->getLayout()->begin(); iterator != _playfield->getLayout()->end(); iterator++) {
		LayoutItem *lprops = &(&*iterator)->second;
		if (strcmp(lprops->n.c_str(), "box") != 0) {
			this->createObject(lprops);
		}
	}

}

void Physics::createObject(LayoutItem *item) {
	
	if (strcmp(item->o->s.c_str(), "box") == 0) {
		this->createBox(item);
		// TODO: goeth elsewhere;
		item->width = (float)(item->v[3].x - item->v[0].x);
		item->height = (float)(item->v[1].y - item->v[0].y);
	} else if (strcmp(item->o->s.c_str(), "segment") == 0) {
		this->createSegment(item);
	} else if (strcmp(item->o->s.c_str(), "flipper") == 0) {
		new Flipper(item, shapeGroupFlippers, _boxBody, this);
	} else if (strcmp(item->o->s.c_str(), "ball") == 0) {
        this->createBall(item);
    } else if (strcmp(item->o->s.c_str(), "switch") == 0) {
        new Switch(item, shapeGroupSwitch, _boxBody, this);
	} else if (strcmp(item->o->s.c_str(), "circle") == 0) {
		this->createCircle(item);
	} else if (strcmp(item->o->s.c_str(), "target") == 0) {
        new Target(item, shapeGroupTargets, _boxBody, this);
	} else if (strcmp(item->o->s.c_str(), "popbumper") == 0) {
        new PopBumper(item, shapeGroupPopbumpers, _boxBody, this);
	} else if (strcmp(item->o->s.c_str(), "slingshot") == 0) {
        new Slingshot(item, shapeGroupSlingshots, _boxBody, this);
    } else if (strcmp(item->o->s.c_str(), "kraken") == 0) {
        new Kraken(item, shapeGroupKraken, _boxBody, this);
    }

}

void Physics::createBox(LayoutItem *item) {

	cpBody *body, *staticBody = cpSpaceGetStaticBody(_space);
	cpShape *shape;
	cpConstraint *constraint;

	cpFloat area = (item->v[1].y - item->v[0].y) * (item->v[3].x - item->v[0].x);
	cpFloat mass = area * item->o->m->d;
	
	// create body on which to hang the "box";
	body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForPoly(mass, 4, (const cpVect *)&item->v[0], cpvzero)));
	
	// the implications of attaching all playfield objects to this non-zero-indexed body are not inconsequential...
	cpBodySetPos(body, cpvmult(cpvadd(item->v[2], item->v[0]), 0.5f));
	
	// pin the box body at the four corners;
	//constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, staticBody, boxVerts[0]));
	//constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, staticBody, boxVerts[1]));
	//constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, staticBody, boxVerts[2]));
	//constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, staticBody, boxVerts[3]));
	
	// pin the box in place;
	constraint = cpSpaceAddConstraint(_space, cpPivotJointNew(body, staticBody, cpvmult(cpvadd(cpvmult(cpvadd(item->v[0], item->v[1]), 0.5), item->v[2]), 0.5)) );
	//constraint = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(body, staticBody, 0.0f, 0.0f));
	constraint = cpSpaceAddConstraint(_space, cpDampedRotarySpringNew(body, staticBody, 0.0f, _boxStiffness, _boxDamping) );
	
	// hang the box shapes on the body;
	// left
	shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpBodyWorld2Local(body, item->v[0]), cpBodyWorld2Local(body, item->v[1]), item->o->r1));
	cpShapeSetElasticity(shape, item->o->m->e);
	cpShapeSetFriction(shape, item->o->m->f);
	cpShapeSetGroup(shape, shapeGroupBox);

	// top
	shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpBodyWorld2Local(body, item->v[1]), cpBodyWorld2Local(body, item->v[2]), item->o->r1));
	cpShapeSetElasticity(shape, item->o->m->e);
	cpShapeSetFriction(shape, item->o->m->f);
	cpShapeSetGroup(shape, shapeGroupBox);

	// right
	shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpBodyWorld2Local(body, item->v[2]), cpBodyWorld2Local(body, item->v[3]), item->o->r1));
	cpShapeSetElasticity(shape, item->o->m->e);
	cpShapeSetFriction(shape, item->o->m->f);
	cpShapeSetGroup(shape, shapeGroupBox);

	// bottom
	shape = cpSpaceAddShape(_space, cpSegmentShapeNew(body, cpBodyWorld2Local(body, item->v[3]), cpBodyWorld2Local(body, item->v[0]), item->o->r1));
	cpShapeSetElasticity(shape, item->o->m->e);
	cpShapeSetFriction(shape, item->o->m->f);
	cpShapeSetGroup(shape, shapeGroupBox);

	cpBodySetUserData(body, item);

	item->bodies.push_back(body);

	_boxBody = body;

}

cpBody *Physics::getBoxBody() {
	return _boxBody;
}

static int ballCollisionGroup = 2048;

static void
ballGravityVelocityFunc(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt)
{
	cpVect g = cpvrotate(cpvforangle(physics_currentInstance->getBoxBody()->a), gravity);
	
	cpBodyUpdateVelocity(body, g, damping, dt);
}

void Physics::createBall(LayoutItem *item) {
    
    cpFloat area = (item->o->r1 * item->s * item->o->r1 * item->s * M_PI);
    cpFloat mass = area * item->o->m->d;
    
    cpBody *body = cpSpaceAddBody(_space, cpBodyNew(mass, cpMomentForCircle(mass, 0, item->o->r1 * item->s, cpvzero)));
    cpBodySetPos(body, item->v[0]);
    body->velocity_func = ballGravityVelocityFunc;
    
    cpShape *shape = cpSpaceAddShape(_space, cpCircleShapeNew(body, item->o->r1 * item->s, cpvzero));
    cpShapeSetElasticity(shape, item->o->m->e);
    cpShapeSetFriction(shape, item->o->m->f);
	cpShapeSetGroup(shape, shapeGroupBall + ballCollisionGroup); // TODO: ball shape group is kludged
	cpShapeSetCollisionType(shape, CollisionTypeBall);
	cpShapeSetUserData(shape, item);

	body->data = item;

	ballCollisionGroup++;

	item->bodies.push_back(body);

}

void Physics::createSegment(LayoutItem *item) {

	LayoutItem *box = &_playfield->getLayout()->find("box")->second;

	for (int i = 0; i < item->count-1; i++) {

		cpShape *shape = cpSpaceAddShape(_space, cpSegmentShapeNew(box->bodies[0], cpBodyWorld2Local(box->bodies[0], item->v[i]), cpBodyWorld2Local(box->bodies[0], item->v[i+1]), item->o->r1));
		cpShapeSetElasticity(shape, item->o->m->e);
		cpShapeSetFriction(shape, item->o->m->f);
		cpShapeSetUserData(shape, item);

		item->shapes.push_back(shape);

	}
	
}

void Physics::createCircle(LayoutItem *item) {

	LayoutItem *box = &_playfield->getLayout()->find("box")->second;

	cpShape *shape = cpSpaceAddShape(_space, cpCircleShapeNew(box->bodies[0], item->o->r1, cpBodyWorld2Local(box->bodies[0], item->v[0])));
	cpShapeSetElasticity(shape, item->o->m->e);
	cpShapeSetFriction(shape, item->o->m->f);
	cpShapeSetUserData(shape, item);

	item->shapes.push_back(shape);
	
}

LayoutItem *Physics::getLayoutItem(const char *itemName) {
    return &_playfield->getLayout()->find(itemName)->second;
}

void Physics::activateMech(const char *mechName) {

	LayoutItem *item = &_playfield->getLayout()->find(mechName)->second;

	if (strcmp(item->o->s.c_str(), "flipper") == 0) {
		flip(item);
	}

}

void Physics::deactivateMech(const char *mechName) {

	LayoutItem *item = &_playfield->getLayout()->find(mechName)->second;

	if (strcmp(item->o->s.c_str(), "flipper") == 0) {
		unflip(item);
	}

}

void Physics::flip(LayoutItem *flipper) {

	cpBodyResetForces(flipper->bodies[0]);

	float dir = (float)(flipper->v[0].x < flipper->v[1].x ? 1 : -1);

	// TODO: precompute
	float offset = (float)cpvlength(cpvsub(flipper->v[0], flipper->v[1]));

	cpVect anchor = cpvadd(flipper->bodies[0]->p, cpvmult(cpv(1, 0), offset));

	cpBodyApplyImpulse(flipper->bodies[0], cpv(0, flipImpulse * dir), anchor);
	cpBodyApplyForce(flipper->bodies[0], cpv(0, flipForce * dir), anchor);

	anchor = cpvadd(flipper->bodies[0]->p, cpvmult(cpv(-1, 0), offset));

	cpBodyApplyImpulse(flipper->bodies[0], cpv(0, -flipImpulse * dir), anchor);
	cpBodyApplyForce(flipper->bodies[0], cpv(0, -flipForce * dir), anchor);

}

void Physics::unflip(LayoutItem *flipper) {

	cpBodyResetForces(flipper->bodies[0]);

	float dir = (float)(flipper->v[0].x < flipper->v[1].x ? -1 : 1);

	// TODO: precompute
	float offset = (float)cpvlength(cpvsub(flipper->v[0], flipper->v[1]));

	cpVect anchor = cpvadd(flipper->bodies[0]->p, cpvmult(cpv(1, 0), offset));

	cpBodyApplyImpulse(flipper->bodies[0], cpv(0, unflipImpulse * dir), anchor);
	cpBodyApplyForce(flipper->bodies[0], cpv(0, unflipForce * dir), anchor);

	anchor = cpvadd(flipper->bodies[0]->p, cpvmult(cpv(-1, 0), offset));

	cpBodyApplyImpulse(flipper->bodies[0], cpv(0, -unflipImpulse * dir), anchor);
	cpBodyApplyForce(flipper->bodies[0], cpv(0, -unflipForce * dir), anchor);

}

void Physics::nudge(cpVect dir) {
	cpBodyApplyImpulse(_boxBody, cpv(_nudgeImpulse * dir.x, _nudgeImpulse * dir.y), cpv(0, -0.1));
	//cpBodyApplyForce(_boxBody, cpv(_nudgeImpulse * dir.x, _nudgeImpulse * dir.y), cpv(0, -01.));
}

void Physics::loadConfig() {

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *configFileName = _bridgeInterface->getScriptPath((const char *)"config.lua");

	int error = luaL_dofile(L, configFileName);
	if (!error) {

        lua_getglobal(L, "config");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				const char *key = lua_tostring(L, -2);
                    
				if (strcmp("timeStep", key) == 0) {
						
                    timeStep = lua_tonumber(L, -1);
                        
				} else if (strcmp("scale", key) == 0) {

					scale = (float)lua_tonumber(L, -1);

				} else if (strcmp("iterations", key) == 0) {

					iterations = (int)lua_tonumber(L, -1);

				}
                    
				lua_pop(L, 1);
			}
            
		}
        
		lua_pop(L, 1); // pop table

    } else {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  // pop err from lua stack
	}

	lua_close(L);

}

void Physics::loadForces() {
    
    lua_State *L = luaL_newstate();
	luaL_openlibs(L);
    
	const char *forcesPath = _bridgeInterface->getScriptPath((const char *)"forces.lua");
    
	int error = luaL_dofile(L, forcesPath);
	if (!error) {
        
        lua_getglobal(L, "forces");
        
		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
					const char *key = lua_tostring(L, -2);
					
					if (strcmp("gravity", key) == 0) {
						
                        // get the first vertex
                        lua_rawgeti(L, -1, 1);
                        gravity.x = (float)lua_tonumber(L, -1);
                        lua_pop(L, 1);
                        
                        // get the second vertex
                        lua_rawgeti(L, -1, 2);
                        gravity.y = (float)lua_tonumber(L, -1);
                        lua_pop(L, 1);
                        
					} else if (strcmp("flipImpulse", key) == 0) {
						flipImpulse = (float)lua_tonumber(L, -1);
					} else if (strcmp("flipForce", key) == 0) {
						flipForce = (float)lua_tonumber(L, -1);
					} else if (strcmp("unflipForce", key) == 0) {
						unflipForce = (float)lua_tonumber(L, -1);
					} else if (strcmp("unflipImpulse", key) == 0) {
						unflipImpulse = (float)lua_tonumber(L, -1);
					} else if (strcmp("nudgeImpulse", key) == 0) {
						_nudgeImpulse = (float)lua_tonumber(L, -1);
					} else if (strcmp("boxStiffness", key) == 0) {
						_boxStiffness = (float)lua_tonumber(L, -1);
					} else if (strcmp("boxDamping", key) == 0) {
						_boxDamping = (float)lua_tonumber(L, -1);
					}
                    
					lua_pop(L, 1);
				}
            
		}
        
		lua_pop(L, 1); // pop table
        
    } else {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  // pop err from lua stack
	}
    
	lua_close(L);
    
}

void Physics::initCollisionHandlers(void) {
    
    cpSpaceAddCollisionHandler(_space, CollisionTypeBall, CollisionTypeBall, NULL, __ballPreSolve, NULL, NULL, NULL);
    
    
}

Physics::~Physics(void)
{
	cpSpaceFree(_space);
}

double currentTime;
double accumulator;

void Physics::updatePhysics() {

    double newTime = absoluteTime();
    double fTime = newTime - currentTime;
	if (fTime > 0.25) {
		fTime = 0.25;
	}
	currentTime = newTime;

    accumulator += fTime;
    
    while (accumulator >= timeStep) {
        cpSpaceStep(_space, timeStep);
        accumulator -= timeStep;
    }
    
	//const double alpha = accumulator / timeStep;

}

void Physics::setPaused(bool paused) {
	_paused = paused;
	if (!_paused) {
		currentTime = absoluteTime();
	}
}

bool Physics::getPaused() {
	return _paused;
}

void Physics::resetBallsToInitialPosition() {
	for (it_LayoutItem iterator = _playfield->getLayout()->begin(); iterator != _playfield->getLayout()->end(); iterator++) {
		LayoutItem item = iterator->second;
		if (strcmp("ball", item.o->s.c_str()) == 0) {
			cpBody *body = item.bodies[0];
			cpBodyResetForces(body);
			cpBodySetPos(body, cpv(item.v[0].x, item.v[0].y));
		}
	}
}

