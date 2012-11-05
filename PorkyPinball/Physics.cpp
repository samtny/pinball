
#include "Physics.h"
#include "chipmunk.h"

extern "C" {
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}

using namespace std;
#include <string>
#include <map>

typedef struct materialProperties {
	float e;
	float f;
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
	float v[200];
	int count;
} layoutItemProperties;
map<string, layoutItemProperties> layoutItems;
typedef map<string, layoutItemProperties>::iterator it_layoutItems;

static cpSpace *space;
static cpVect gravity = cpv(0.0, 9.80665f);

static cpFloat timeStep = 1.0/180.0;

Physics::Physics(void)
{
	
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
		


}

void Physics::loadMaterials() {
	
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	int error = luaL_dofile(L, "materials.lua");
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

	int error = luaL_dofile(L, "objects.lua");
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

	int error = luaL_dofile(L, "layout.lua");
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
						
						for (int i = 1; i <= length; i++)
						{
							lua_rawgeti(L, -1, i);
							props.v[i-1] = (float)lua_tonumber(L, -1);
							lua_pop(L, 1);
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