
#include "Physics.h"
#include "chipmunk.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "PinballNativeInterface.h"

typedef struct materialProperties {
	float e;
	float f;
	float d;
} materialProperties;
map<string, materialProperties> materials;
typedef map<string, materialProperties>::iterator it_materialProperties;

typedef struct objectProperties {
	string s;
	string m;
	float r1;
	float r2;
} objectProperties;
map<string, objectProperties> objects;
typedef map<string, objectProperties>::iterator it_objectProps;

typedef struct layoutItemProperties {
	string o;
	cpVect v[200];
	int count;
} layoutItemProperties;
map<string, layoutItemProperties> layoutItems;
typedef map<string, layoutItemProperties>::iterator it_layoutItems;

static cpSpace *space;

static cpVect gravity = cpv(0.0, 9.80665f);

static cpFloat timeStep = 1.0/180.0;

enum shapeGroup {
	shapeGroupBox,
	shapeGroupFlippers
};

Physics::Physics(void)
{
	
}

void Physics::init(PinballNativeImpl *pinballNative) {

	this->_pinballNativeImpl = pinballNative;

	this->loadMaterials();
	this->loadObjects();
	this->loadLayout();

	space = cpSpaceNew();
	
	for (it_layoutItems iterator = layoutItems.begin(); iterator != layoutItems.end(); iterator++) {
		string name = iterator->first;
		layoutItemProperties lprops = iterator->second;
		objectProperties oprops = objects[lprops.o];
		materialProperties mprops = materials[oprops.m];
		this->createObject(name, lprops, oprops, mprops);
	}

	cpSpaceSetGravity(space, gravity);

}

void Physics::createObject(string name, layoutItemProperties layoutItem, objectProperties object, materialProperties material) {
	
	if (strcmp(object.s.c_str(), "box") == 0) {
		this->createBox(name, layoutItem, object, material);
	} else if (strcmp(object.s.c_str(), "segment") == 0) {
		this->createSegment(name, layoutItem, object, material);
	} else if (strcmp(object.s.c_str(), "flipper") == 0) {
		this->createFlipper(name, layoutItem, object, material);
	}

}

void Physics::createBox(string name, layoutItemProperties item, objectProperties object, materialProperties material) {

	cpBody *body, *staticBody = cpSpaceGetStaticBody(space);
	cpShape *shape;
	cpConstraint *constraint;

	cpFloat area = (item.v[1].y - item.v[0].y) * (item.v[3].x - item.v[0].x);
	cpFloat mass = area * material.d;

	// create body on which to hang the "box";
	body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForPoly(mass, 4, item.v, cpvzero)));
	
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
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, item.v[0], item.v[1], object.r1));
	cpShapeSetElasticity(shape, material.e);
	cpShapeSetFriction(shape, material.f);
	cpShapeSetGroup(shape, shapeGroupBox);

	// top
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, item.v[1], item.v[2], object.r1));
	cpShapeSetElasticity(shape, material.e);
	cpShapeSetFriction(shape, material.f);
	cpShapeSetGroup(shape, shapeGroupBox);

	// right
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, item.v[2], item.v[3], object.r1));
	cpShapeSetElasticity(shape, material.e);
	cpShapeSetFriction(shape, material.f);
	cpShapeSetGroup(shape, shapeGroupBox);

	// bottom
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, item.v[3], item.v[0], object.r1));
	cpShapeSetElasticity(shape, material.e);
	cpShapeSetFriction(shape, material.f);
	cpShapeSetGroup(shape, shapeGroupBox);

	//box = staticBody;
	_box = body;
	
}

void Physics::createFlipper(string name, layoutItemProperties item, objectProperties object, materialProperties material) {

	cpFloat area = (object.r1 * M_PI) * 2; // approx
	cpFloat mass = area * material.d;

	cpFloat direction = item.v[0].x <= item.v[1].x ? -1 : 1; // rotate clockwise for right-facing flipper...
	cpFloat length = cpvdist(item.v[0], item.v[1]);
	cpFloat flipAngle = direction * cpfacos(cpvdot(cpvnormalize(cpvsub(item.v[1],item.v[0])), cpvnormalize(cpvsub(item.v[2],item.v[0]))));

	// flipper body is round centered at base of flipper, and for this implementation has radius == flipper length;
	cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForCircle(mass, 0.0f, length, cpvzero)));
	cpBodySetPos(body, item.v[0]);

	cpConstraint *constraint = cpSpaceAddConstraint(space, cpPivotJointNew(body, _box, item.v[0]));
	constraint = cpSpaceAddConstraint(space, cpRotaryLimitJointNew(body, _box, flipAngle, 0.0f));

	// lflipper base shape
	cpShape *shape = cpSpaceAddShape(space, cpCircleShapeNew(body, object.r1, cpvzero));
	cpShapeSetElasticity(shape, material.e);
	cpShapeSetFriction(shape, material.f);
	cpShapeSetGroup(shape, shapeGroupFlippers);

	// lflipper face shape
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, cpvsub(item.v[0], body->p), cpvsub(item.v[1], body->p), object.r2));
	cpShapeSetElasticity(shape, material.e);
	cpShapeSetFriction(shape, material.f);
	cpShapeSetGroup(shape, shapeGroupFlippers);

}

void Physics::createSegment(string name, layoutItemProperties layoutItem, objectProperties object, materialProperties material) {

	//...

}

void Physics::loadMaterials() {
	
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *materialsFileName = _pinballNativeImpl->getPathForScriptFileName("materials.lua");

	int error = luaL_dofile(L, materialsFileName);
	if (!error) {

        lua_getglobal(L, "materials");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				// key;
				const char *name = lua_tostring(L, -2);

				materialProperties props = { -1, -1 };

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

	const char *objectsPath = _pinballNativeImpl->getPathForScriptFileName("objects.lua");

	int error = luaL_dofile(L, objectsPath);
	if (!error) {

        lua_getglobal(L, "objects");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				// key;
				const char *name = lua_tostring(L, -2);

				objectProperties props = { "", "", -1, -1 };

				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {
					
					const char *key = lua_tostring(L, -2);

					if (strcmp("s", key) == 0) {
						props.s = lua_tostring(L, -1);
					} else if (strcmp("m", key) == 0) {
						props.m = lua_tostring(L, -1);
					} else if (strcmp("r1", key) == 0) {
						props.r1 = (float)lua_tonumber(L, -1);
					} else if (strcmp("r2", key) == 0) {
						props.r2 = (float)lua_tonumber(L, -1);
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

	const char *layoutPath = _pinballNativeImpl->getPathForScriptFileName("layout.lua");

	int error = luaL_dofile(L, layoutPath);
	if (!error) {

        lua_getglobal(L, "layout");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				// key;
				const char *name = lua_tostring(L, -2);

				layoutItemProperties props = { "", NULL };

				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {
					
					const char *key = lua_tostring(L, -2);

					if (strcmp("o", key) == 0) {
						
						props.o = lua_tostring(L, -1);

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

Physics::~Physics(void)
{
	cpSpaceFree(space);
}

void Physics::updatePhysics() {

	cpSpaceStep(space, timeStep);

}