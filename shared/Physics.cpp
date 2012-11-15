
#include "Physics.h"
#include "chipmunk/chipmunk.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "PinballBridgeInterface.h"

static cpSpace *space;

static cpVect gravity = cpv(0.0, 9.80665f);

static cpFloat timeStep = 1.0/180.0;

static cpFloat scale = 37;

enum shapeGroup {
	shapeGroupBox,
    shapeGroupBall,
	shapeGroupFlippers
};

#ifdef _WIN32

static double absoluteTime() {
    return timeGetTime() / (double) 1000;
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
	
}

void Physics::setBridgeInterface(PinballBridgeInterface *bridgeInterface) {
	this->_bridgeInterface = bridgeInterface;
}

void Physics::init() {

	this->loadConfig();
	this->loadMaterials();
	this->loadObjects();
	this->loadLayout();
    this->loadForces();

	space = cpSpaceNew();
	
	for (it_layoutItems iterator = layoutItems.begin(); iterator != layoutItems.end(); iterator++) {
		layoutItemProperties *lprops = &(&*iterator)->second;
		this->applyScale(lprops);
		this->createObject(lprops);
	}
	/*
	for (it_layoutItems iterator = layoutItems.begin(); iterator != layoutItems.end(); iterator++) {
		layoutItemProperties lprops = iterator->second;
		this->applyScale(&lprops);
		this->createObject(&lprops);
	}
	*/
	cpSpaceSetGravity(space, gravity);

}

float Physics::getBoxWidth() {
	return _boxWidth;
}

cpSpace *Physics::getSpace() {
	return space;
}

void Physics::applyScale(layoutItemProperties *iprops) {

	for (int i = 0; i < iprops->count; i++) {
		iprops->v[i].x *= 1 / scale;
		iprops->v[i].y *= 1 / scale;
	}

	iprops->o.r1 *= 1 / scale;
	iprops->o.r2 *= 1 / scale;
	
	//iprops->o.t.s *= 1 / scale;

}

void Physics::createObject(layoutItemProperties *layoutItem) {
	
	if (strcmp(layoutItem->o.s.c_str(), "box") == 0) {
		this->createBox(layoutItem);
	} else if (strcmp(layoutItem->o.s.c_str(), "segment") == 0) {
		this->createSegment(layoutItem);
	} else if (strcmp(layoutItem->o.s.c_str(), "flipper") == 0) {
		this->createFlipper(layoutItem);
	} else if (strcmp(layoutItem->o.s.c_str(), "ball") == 0) {
        layoutItem->body = this->createBall(layoutItem);
    }

}

void Physics::createBox(layoutItemProperties *item) {

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

	//box = staticBody;
	_box = body;
	_boxWidth = item->v[3].x - item->v[0].x;
	
}

cpBody *Physics::createBall(layoutItemProperties *item) {
    
    cpFloat area = (item->o.r1 * M_PI) * 2;
    cpFloat mass = area * item->o.m.d;
    
    cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForCircle(mass, 0, item->o.r1, cpvzero)));
    cpBodySetPos(body, item->v[0]);
    
    cpShape *shape = cpSpaceAddShape(space, cpCircleShapeNew(body, item->o.r1, cpvzero));
    cpShapeSetElasticity(shape, item->o.m.e);
    cpShapeSetFriction(shape, item->o.m.f);
	cpShapeSetGroup(shape, shapeGroupBall);
    
	// TODO: something elsewise;
	_balls[0] = body;

	return body;

}

void Physics::createFlipper(layoutItemProperties *item) {

	cpFloat area = (item->o.r1 * M_PI) * 2; // approx
	cpFloat mass = area * item->o.m.d;

	cpFloat direction = item->v[0].x <= item->v[1].x ? -1 : -1; // rotate clockwise for right-facing flipper...

	cpFloat length = cpvdist(item->v[0], item->v[1]);
	cpFloat flipAngle = direction * cpfacos(cpvdot(cpvnormalize(cpvsub(item->v[1],item->v[0])), cpvnormalize(cpvsub(item->v[2],item->v[0]))));

	// flipper body is round centered at base of flipper, and for this implementation has radius == flipper length;
	cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForCircle(mass, 0.0f, length, cpvzero)));
	cpBodySetPos(body, item->v[0]);

	cpConstraint *constraint = cpSpaceAddConstraint(space, cpPivotJointNew(body, _box, item->v[0]));
	constraint = cpSpaceAddConstraint(space, cpRotaryLimitJointNew(body, _box, flipAngle, 0.0f));

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

}

void Physics::createSegment(layoutItemProperties *layoutItem) {

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

						scale = lua_tonumber(L, -1);

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
				
				materials.insert(make_pair(name, props));

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

				objectProperties props = { "", "", -1, -1, { "", -1, -1, -1, }, { "", -1, -1, -1, -1 } };

				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {
					
					const char *key = lua_tostring(L, -2);

					if (strcmp("s", key) == 0) {
						props.s = lua_tostring(L, -1);
					} else if (strcmp("m", key) == 0) {
						props.m = materials[lua_tostring(L, -1)];
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
				
				objects.insert(make_pair(name, props));

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

				layoutItemProperties props = { "" };

				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {
					
					const char *key = lua_tostring(L, -2);

					if (strcmp("o", key) == 0) {
						
						props.o = objects[lua_tostring(L, -1)];

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

					}

					lua_pop(L, 1);
				}
				
				layoutItems.insert(make_pair(name, props));

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
                    
					if (strcmp("timeStep", key) == 0) {
						
                        timeStep = lua_tonumber(L, -1);
                        
					} else if (strcmp("gravity", key) == 0) {
						
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

void Physics::resetBallPosition(int ballIndex) {
	cpBody *ball = _balls[ballIndex];
	cpBodyResetForces(ball);
	// TODO: something way, way better;
	cpBodySetPos(ball, cpv(0.1, 0.65));
}

Physics::~Physics(void)
{
	cpSpaceFree(space);
}

double currentTime;
double newTime;
double accumulator;

void Physics::updatePhysics() {

    double newTime = absoluteTime();
    
    if (currentTime == 0) { currentTime = newTime; return; }
    
    double fTime = newTime - currentTime;
    accumulator += fTime;
    
    while (accumulator >= timeStep) {
        cpSpaceStep(space, timeStep);
        accumulator -= timeStep;
    }
    
    currentTime = newTime;
    
}






