
#include "Playfield.h"

#include "PinballBridgeInterface.h"

#include "Parts.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

void Playfield::setBridgeInterface(PinballBridgeInterface *bridgeInterface) {
	_bridgeInterface = bridgeInterface;
}

map<string, Material> *Playfield::getMaterials() {
	return &_materials;
}

map<string, LayoutItem> *Playfield::getLayout() {
	return &_layout;
}

void Playfield::init(void) {

	this->loadConfig();
	this->loadMaterials();
	this->loadTextures();
	this->loadParts();
	this->loadLayout();

}

void Playfield::loadConfig(void) {



}

void Playfield::loadMaterials(void) {
	
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

				Material m;

				// "value" is properties table;
				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {

					// property name
					const char *key = lua_tostring(L, -2);

					// property value
					float val = (float)lua_tonumber(L, -1);

					if (strcmp("e", key) == 0) {
						m.e = val;
					} else if (strcmp("f", key) == 0) {
						m.f = val;
					} else if (strcmp("d", key) == 0) {
						m.d = val;
					}

					lua_pop(L, 1);
				}
				
				_materials.insert(make_pair(name, m));

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

void Playfield::loadTextures(void) {

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *texturesFileName = _bridgeInterface->getPathForScriptFileName((void *)"textures.lua");

	int error = luaL_dofile(L, texturesFileName);
	if (!error) {

        lua_getglobal(L, "textures");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				const char *name = lua_tostring(L, -2);

				Texture props;
				props.n = name;
				
				lua_pushnil(L);
				while (lua_next(L, -2) != 0) {

					const char *key = lua_tostring(L, -2);
                    
					if (strcmp("filename", key) == 0) {
						
						props.filename = lua_tostring(L, -1);
                        
					}
                    
					lua_pop(L, 1);
				}

				_textures.insert(make_pair(name, props));

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

void Playfield::loadParts(void) {

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *objectsPath = _bridgeInterface->getPathForScriptFileName((void *)"parts.lua");

	int error = luaL_dofile(L, objectsPath);
	if (!error) {

        lua_getglobal(L, "parts");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				// key;
				const char *name = lua_tostring(L, -2);

				Part props;
				props.n = name;

				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {
					
					const char *key = lua_tostring(L, -2);

					if (strcmp("s", key) == 0) {
						props.s = lua_tostring(L, -1);
					} else if (strcmp("m", key) == 0) {
						props.m = &_materials[lua_tostring(L, -1)];
					} else if (strcmp("r1", key) == 0) {
						props.r1 = (float)lua_tonumber(L, -1);
					} else if (strcmp("r2", key) == 0) {
						props.r2 = (float)lua_tonumber(L, -1);
					} else if (strcmp("t", key) == 0) {

						lua_pushnil(L);
						while(lua_next(L, -2) != 0) {
							
							const char *tkey = lua_tostring(L, -2);

							if (strcmp("n", tkey) == 0) {
								props.t.t = &_textures[lua_tostring(L, -1)];
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
				
				_parts.insert(make_pair(name, props));

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

void Playfield::loadLayout(void) {
	
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

				LayoutItem props;
				props.n = name;
				props.s = -1;

				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {
					
					const char *key = lua_tostring(L, -2);

					if (strcmp("o", key) == 0) {
						
						props.o = &_parts[lua_tostring(L, -1)];

					} else if (strcmp("v", key) == 0) {
						
						int length = lua_rawlen(L, -1);
						
						// traverse 2d vects
						for (int i = 1; i <= length; i++)
						{

							// init vect object
							Coord2 v;

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

				_layout.insert(make_pair(name, props));

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

