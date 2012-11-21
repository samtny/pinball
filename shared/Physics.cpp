
#include "Physics.h"

#include "PinballBridgeInterface.h"

#include "PhysicsDelegate.h"

#include "Parts.h"

#include "chipmunk/chipmunk.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

static Physics *physics_currentInstance;

static cpSpace *space;

static cpVect gravity = cpv(0.0, 9.80665f);

static double timeStep = 1.0/180.0;

static float scale = 37;

enum shapeGroup {
	shapeGroupBox,
    shapeGroupBall,
	shapeGroupFlippers
};

enum CollisionType {
	CollisionTypeNone,
	CollisionTypeSwitch,
	CollisionTypeBall
};

#ifdef _WIN32

static double absoluteTime() {
    //return timeGetTime() / (double) 1000;
	// hi-res timer courtesy gafferongames.com;
	static __int64 start = 0;
    static __int64 frequency = 0;

    if (start==0)
    {
        QueryPerformanceCounter((LARGE_INTEGER*)&start);
        QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
        return 0.0f;
    }

    __int64 counter = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&counter);
    return (float) ((counter - start) / double(frequency));
}

#elif __APPLE__

#import <mach/mach_time.h>

static double absoluteTime() {
    static double sysTimebaseMult = -1;
    if (sysTimebaseMult == -1) {
        mach_timebase_info_data_t info;
        kern_return_t err = mach_timebase_info(&info);
        if (err == 0) {
            sysTimebaseMult = 1e-9 * (double) info.numer / (double) info.denom;
        }
    }
    return mach_absolute_time() * sysTimebaseMult;
}

#endif

Physics::Physics(void)
{
	physics_currentInstance = this;
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

map<string, layoutItem> *Physics::getLayoutItems() {
	return &_layoutItems;
}

void Physics::init() {

	this->loadConfig();
	this->loadMaterials();
	this->loadObjects();
	this->loadLayout();
    this->loadForces();

	space = cpSpaceNew();
	
	for (it_layoutItems iterator = _layoutItems.begin(); iterator != _layoutItems.end(); iterator++) {
		layoutItem *lprops = &(&*iterator)->second;
		this->applyScale(lprops);
		this->createObject(lprops);
	}

	/*
	for (it_layoutItems iterator = layoutItems.begin(); iterator != layoutItems.end(); iterator++) {
		layoutItem lprops = iterator->second;
		this->applyScale(&lprops);
		this->createObject(&lprops);
	}
	*/

	this->initCollisionHandlers();

	cpSpaceSetGravity(space, gravity);

}

static int __ballPreSolve(cpArbiter *arb, cpSpace *space, void *unused) {

	return physics_currentInstance->ballPreSolve(arb, space, unused);

}

int Physics::ballPreSolve(cpArbiter *arb, cpSpace *space, void *unused) {

	materialProperties *mat = &_materials["steel"];

	arb->e = mat->e;
	arb->u = mat->f;

    _bridgeInterface->playSound((void *)"flip");
    
	return 1;

}

static int switchBegin(cpArbiter *arb, cpSpace *space, void *unused) {

	cpShape *a;
	cpShape *b;
	cpArbiterGetShapes(arb, &a, &b);

	layoutItem *l = (layoutItem *)b->data;

	physics_currentInstance->getDelegate()->switchClosed(l->n.c_str());

	return 1;

}

static void switchSeparate(cpArbiter *arb, cpSpace *space, void *unused) {



}

void Physics::initCollisionHandlers(void) {

	cpSpaceAddCollisionHandler(space, CollisionTypeBall, CollisionTypeBall, NULL, __ballPreSolve, NULL, NULL, NULL);
	cpSpaceAddCollisionHandler(space, CollisionTypeBall, CollisionTypeSwitch, switchBegin, NULL, NULL, switchSeparate, NULL);

}

cpSpace *Physics::getSpace() {
	return space;
}

void Physics::applyScale(layoutItem *iprops) {

	for (int i = 0; i < iprops->count; i++) {
		iprops->v[i].x *= 1 / scale;
		iprops->v[i].y *= 1 / scale;
	}

	float localScale = scale * 1 / iprops->s;

	iprops->o.r1 *= 1 / localScale;
	iprops->o.r2 *= 1 / localScale;
	
	//iprops->o.t.s *= 1 / scale;

}

void Physics::createObject(layoutItem *layoutItem) {
	
	if (strcmp(layoutItem->o.s.c_str(), "box") == 0) {
		layoutItem->body = this->createBox(layoutItem);
		layoutItem->width = (float)(layoutItem->v[3].x - layoutItem->v[0].x);
		layoutItem->height = (float)(layoutItem->v[1].y - layoutItem->v[0].y);
	} else if (strcmp(layoutItem->o.s.c_str(), "segment") == 0) {
		this->createSegment(layoutItem);
	} else if (strcmp(layoutItem->o.s.c_str(), "flipper") == 0) {
		layoutItem->body = this->createFlipper(layoutItem);
	} else if (strcmp(layoutItem->o.s.c_str(), "ball") == 0) {
        layoutItem->body = this->createBall(layoutItem);
    } else if (strcmp(layoutItem->o.s.c_str(), "switch") == 0) {
		this->createSwitch(layoutItem);
	}

}

cpBody *Physics::createBox(layoutItem *item) {

	cpBody *body, *staticBody = cpSpaceGetStaticBody(space);
	cpShape *shape;
	cpConstraint *constraint;

	cpFloat area = (item->v[1].y - item->v[0].y) * (item->v[3].x - item->v[0].x);
	cpFloat mass = area * item->o.m.d;

	// create body on which to hang the "box";
	body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForPoly(mass, 4, item->v, cpvzero)));
	
	// pin the box body at the four corners;
	//constraint = cpSpaceAddConstraint(space, cpPivotJointNew(body, staticBody, boxVerts[0]));
	//constraint = cpSpaceAddConstraint(space, cpPivotJointNew(body, staticBody, boxVerts[1]));
	//constraint = cpSpaceAddConstraint(space, cpPivotJointNew(body, staticBody, boxVerts[2]));
	//constraint = cpSpaceAddConstraint(space, cpPivotJointNew(body, staticBody, boxVerts[3]));
	
	// pin the box in place;
	constraint = cpSpaceAddConstraint(space, cpPivotJointNew(body, staticBody, cpvzero));
	constraint = cpSpaceAddConstraint(space, cpRotaryLimitJointNew(body, staticBody, 0.0f, 0.0f));
	
	// hang the box shapes on the body;
	// left
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, item->v[0], item->v[1], item->o.r1));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupBox);

	// top
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, item->v[1], item->v[2], item->o.r1));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupBox);

	// right
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, item->v[2], item->v[3], item->o.r1));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupBox);

	// bottom
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, item->v[3], item->v[0], item->o.r1));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupBox);

	return body;

}

static int ballCollisionGroup = 2048;

cpBody *Physics::createBall(layoutItem *item) {
    
    cpFloat area = (item->o.r1 * M_PI) * 2;
    cpFloat mass = area * item->o.m.d;
    
    cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForCircle(mass, 0, item->o.r1, cpvzero)));
    cpBodySetPos(body, item->v[0]);
    
    cpShape *shape = cpSpaceAddShape(space, cpCircleShapeNew(body, item->o.r1, cpvzero));
    cpShapeSetElasticity(shape, item->o.m.e);
    cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupBall + ballCollisionGroup); // TODO: ball shape group is kludged
	cpShapeSetCollisionType(shape, CollisionTypeBall);

	body->data = item;

	ballCollisionGroup++;

	return body;

}

cpBody *Physics::createFlipper(layoutItem *item) {

	cpFloat area = (item->o.r1 * M_PI) * 2; // approx
	cpFloat mass = area * item->o.m.d;

	cpFloat direction = item->v[0].x <= item->v[1].x ? -1 : -1; // rotate clockwise for right-facing flipper...

	cpFloat length = cpvdist(item->v[0], item->v[1]);
	cpFloat flipAngle = direction * cpfacos(cpvdot(cpvnormalize(cpvsub(item->v[1],item->v[0])), cpvnormalize(cpvsub(item->v[2],item->v[0]))));

	// flipper body is round centered at base of flipper, and for this implementation has radius == flipper length;
	cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForCircle(mass, 0.0f, length, cpvzero)));
	cpBodySetPos(body, item->v[0]);

	layoutItem *box = &_layoutItems.find("box")->second;

 	cpConstraint *constraint = cpSpaceAddConstraint(space, cpPivotJointNew(body, box->body, item->v[0]));
	constraint = cpSpaceAddConstraint(space, cpRotaryLimitJointNew(body, box->body, flipAngle, 0.0f));

	// lflipper base shape
	cpShape *shape = cpSpaceAddShape(space, cpCircleShapeNew(body, item->o.r1, cpvzero));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupFlippers);

	// lflipper face shape
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, cpvsub(item->v[0], body->p), cpvsub(item->v[1], body->p), item->o.r2));
	cpShapeSetElasticity(shape, item->o.m.e);
	cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupFlippers);

	return body;

}

void Physics::createSwitch(layoutItem *item) {

	cpShape *shape = cpSpaceAddShape(space, cpSegmentShapeNew(space->staticBody, item->v[0], item->v[1], item->o.r1));
	cpShapeSetSensor(shape, true);
	cpShapeSetCollisionType(shape, CollisionTypeSwitch);
	cpShapeSetUserData(shape, item);
	
}

void Physics::createSegment(layoutItem *layoutItem) {

	//...

}

void Physics::loadConfig() {

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *configFileName = _bridgeInterface->getPathForScriptFileName((void *)"config.lua");

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

void Physics::loadMaterials() {
	
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *materialsFileName = _bridgeInterface->getPathForScriptFileName((void *)"materials.lua");

	int error = luaL_dofile(L, materialsFileName);
	if (!error) {

        lua_getglobal(L, "materials");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				// key;
				const char *name = lua_tostring(L, -2);

				materialProperties props = { "", -1, -1, -1 };

				// "value" is properties table;
				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {

					// property name
					const char *key = lua_tostring(L, -2);

					// property value
					float val = (float)lua_tonumber(L, -1);

					if (strcmp("e", key) == 0) {
						props.e = val;
					} else if (strcmp("f", key) == 0) {
						props.f = val;
					} else if (strcmp("d", key) == 0) {
						props.d = val;
					}

					lua_pop(L, 1);
				}
				
				_materials.insert(make_pair(name, props));

				lua_pop(L, 1);
			}

		}

		lua_pop(L, 1); // pop materials table

    } else {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  // pop err from lua stack
	}

	lua_close(L);

}

void Physics::loadObjects() {

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *objectsPath = _bridgeInterface->getPathForScriptFileName((void *)"objects.lua");

	int error = luaL_dofile(L, objectsPath);
	if (!error) {

        lua_getglobal(L, "objects");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				// key;
				const char *name = lua_tostring(L, -2);

				objectProperties props = { name, "", -1, -1, { "", -1, -1, -1, }, { "", -1, -1, -1, -1 } };

				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {
					
					const char *key = lua_tostring(L, -2);

					if (strcmp("s", key) == 0) {
						props.s = lua_tostring(L, -1);
					} else if (strcmp("m", key) == 0) {
						props.m = _materials[lua_tostring(L, -1)];
					} else if (strcmp("r1", key) == 0) {
						props.r1 = (float)lua_tonumber(L, -1);
					} else if (strcmp("r2", key) == 0) {
						props.r2 = (float)lua_tonumber(L, -1);
					} else if (strcmp("t", key) == 0) {

						lua_pushnil(L);
						while(lua_next(L, -2) != 0) {
							
							const char *tkey = lua_tostring(L, -2);

							if (strcmp("n", tkey) == 0) {
								props.t.n = lua_tostring(L, -1);
							} else if (strcmp("x", tkey) == 0) {
								props.t.x = (int)lua_tonumber(L, -1);
							} else if (strcmp("y", tkey) == 0) {
								props.t.y = (int)lua_tonumber(L, -1);
							} else if (strcmp("w", tkey) == 0) {
								props.t.w = (int)lua_tonumber(L, -1);
							} else if (strcmp("h", tkey) == 0) {
								props.t.h = (int)lua_tonumber(L, -1);
							} else if (strcmp("a", tkey) == 0) {
								props.t.a = (float)lua_tonumber(L, -1);
							}

							lua_pop(L, 1);

						}

					}

					lua_pop(L, 1);
				}
				
				_objects.insert(make_pair(name, props));

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

void Physics::loadLayout() {
	
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *layoutPath = _bridgeInterface->getPathForScriptFileName((void *)"layout.lua");

	int error = luaL_dofile(L, layoutPath);
	if (!error) {

        lua_getglobal(L, "layout");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				// key;
				const char *name = lua_tostring(L, -2);

				layoutItem props = { name };
				props.s = -1;

				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {
					
					const char *key = lua_tostring(L, -2);

					if (strcmp("o", key) == 0) {
						
						props.o = _objects[lua_tostring(L, -1)];

					} else if (strcmp("v", key) == 0) {
						
						int length = lua_rawlen(L, -1);
						
						// traverse 2d vects
						for (int i = 1; i <= length; i++)
						{

							// init vect object
							cpVect v;

							// get the 2d table
							lua_rawgeti(L, -1, i);

							// get the first vertex
							lua_rawgeti(L, -1, 1);
							v.x = (float)lua_tonumber(L, -1);
							lua_pop(L, 1);

							// get the second vertex
							lua_rawgeti(L, -1, 2);
							v.y = (float)lua_tonumber(L, -1);
							lua_pop(L, 1);
							
							// pop the table;
							lua_pop(L, 1);
							
							// assign vect to array
							props.v[i-1] = v;

						}

						props.count = length;

					} else if (strcmp("s", key) == 0) {
						props.s = (float)lua_tonumber(L, -1);
					}

					lua_pop(L, 1);
				}
				
				if (props.s == -1) {
					props.s = 1;
				}

				_layoutItems.insert(make_pair(name, props));

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
    
	const char *forcesPath = _bridgeInterface->getPathForScriptFileName((void *)"forces.lua");
    
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

Physics::~Physics(void)
{
	cpSpaceFree(space);
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
        cpSpaceStep(space, timeStep);
        accumulator -= timeStep;
    }
    
	//const double alpha = accumulator / timeStep;

}

void Physics::resetBallsToInitialPosition() {
	for (it_layoutItems iterator = _layoutItems.begin(); iterator != _layoutItems.end(); iterator++) {
		layoutItem item = iterator->second;
		if (strcmp("ball", item.o.n.c_str()) == 0) {
			cpBody *body = item.body;
			cpBodyResetForces(body);
			cpBodySetPos(body, cpv(item.v[0].x, item.v[0].y));
		}
	}
}