#include "Camera.h"

#include "Renderer.h"

#include "Physics.h"

#include "Parts.h"

#include "Util.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#ifdef __APPLE__
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#else
	#ifdef _WIN32
		#include <windows.h>
	#endif
	#include <GL/GL.h>
	#include <GL/GLU.h>
#endif

#include <string>
#include <map>

struct CameraMode {
	string name;
	CameraType t;
	Coord2 c;
	Coord2 b; // buffer / border
	float z;
};
map<string, CameraMode> _cameraModes;
typedef map<string, CameraMode>::iterator it_cameraModes;
CameraMode _activeCameraMode;

static float scale = 37;

Camera::Camera()
{

}

Camera::~Camera() {
}

void Camera::setBridgeInterface(PinballBridgeInterface *bridgeInterface) {
	_bridgeInterface = bridgeInterface;
}

void Camera::init() {
	
	this->loadConfig();
	this->loadCamera();
	this->loadEffects();

	for (it_cameraModes iterator = _cameraModes.begin(); iterator != _cameraModes.end(); iterator++) {
		CameraMode *mode = &(&*iterator)->second;
		mode->b.x *= 1 / scale;
		mode->b.y *= 1 / scale;
		mode->c.x *= 1 / scale;
		mode->c.y *= 1 / scale;
	}

}

void Camera::loadConfig() {

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
                    
				if (strcmp("scale", key) == 0) {

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

void Camera::loadCamera() {

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *configFileName = _bridgeInterface->getPathForScriptFileName((void *)"camera.lua");

	int error = luaL_dofile(L, configFileName);
	if (!error) {

        lua_getglobal(L, "camera");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				const char *name = lua_tostring(L, -2);
                    
				CameraMode mode;
				mode.name = name;

				lua_pushnil(L);
				while (lua_next(L, -2) != 0) {

					const char *key = lua_tostring(L, -2);

					if (strcmp(key, "t") == 0) {
						string val = lua_tostring(L, -1);
						if (val.compare("follow") == 0) {
							mode.t = CAMERA_TYPE_FOLLOW_BALL;
						} else if (val.compare("fixed") == 0) {
							mode.t = CAMERA_TYPE_FIXED;
						}
					} else if (strcmp(key, "z") == 0) {
						mode.z = (float)lua_tonumber(L, -1);
					} else if (strcmp(key, "c") == 0) {

						lua_pushnil(L);

						lua_next(L, -2);
						mode.c.x = (float)lua_tonumber(L, -1);
						lua_pop(L, 1);
						lua_next(L, -2);
						mode.c.y = (float)lua_tonumber(L, -1);
						lua_pop(L, 1);

						lua_pop(L, 1);

					} else if (strcmp(key, "b") == 0) {

						lua_pushnil(L);

						lua_next(L, -2);
						mode.b.x = (float)lua_tonumber(L, -1);
						lua_pop(L, 1);
						lua_next(L, -2);
						mode.b.y = (float)lua_tonumber(L, -1);
						lua_pop(L, 1);

						lua_pop(L, 1);

					}

					lua_pop(L, 1);

				}
                    
				lua_pop(L, 1);

				_cameraModes[name] = mode;

			}
            
		}
        
		lua_pop(L, 1); // pop table

    } else {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  // pop err from lua stack
	}

	lua_close(L);

}

void Camera::loadEffects() {

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *effectsFileName = _bridgeInterface->getPathForScriptFileName((void *)"effects.lua");

	int error = luaL_dofile(L, effectsFileName);
	if (!error) {

        lua_getglobal(L, "effects");

		if (lua_istable(L, -1)) {

			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				const char *name = lua_tostring(L, -2);
                    
				cameraEffect effect;
				effect.n = name;

				lua_pushnil(L);
				while (lua_next(L, -2) != 0) {

					const char *key = lua_tostring(L, -2);

					if (strcmp(key, "d") == 0) {
						effect.d = (float)lua_tonumber(L, -1);
					} else if (strcmp(key, "start") == 0) {

						lua_pushnil(L);
						while (lua_next(L, -2) != 0) {

							const char *subkey = lua_tostring(L, -2);

							if (strcmp(subkey, "a") == 0) {
								effect.aStart = (float)lua_tonumber(L, -1);
							}

							lua_pop(L, 1);
							
						}

					} else if (strcmp(key, "finish") == 0) {


						lua_pushnil(L);
						while (lua_next(L, -2) != 0) {

							const char *subkey = lua_tostring(L, -2);

							if (strcmp(subkey, "a") == 0) {
								effect.aEnd = (float)lua_tonumber(L, -1);
							}

							lua_pop(L, 1);
							
						}


					}

					lua_pop(L, 1);
					
				}

				_effects[name] = effect;
				
				lua_pop(L, 1);

			}

		}

		lua_pop(L, 1);

	} else {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  // pop err from lua stack
	}

	lua_close(L);

}

void Camera::setDisplayProperties(HostProperties *displayProperties) {
	_displayProperties = displayProperties;
}

void Camera::setPhysics(Physics *physics) {
	_physics = physics;
}

void Camera::setZoomLevel(float zoomLevel) {
	if (_activeCameraMode.z <= maxZoomLevel && zoomLevel >= minZoomLevel) {
		_activeCameraMode.z = zoomLevel;
	}
}

float Camera::getZoomLevel() {
	return _activeCameraMode.z;
}

void Camera::setWorldScale(float worldScale) {
	_worldScale = worldScale;
}

void Camera::setMode(const char *modeName) {

	for (it_cameraModes iterator = _cameraModes.begin(); iterator != _cameraModes.end(); iterator++) {
		CameraMode mode = iterator->second;
		if (strcmp(modeName, mode.name.c_str()) == 0) {
			_activeCameraMode = mode;
			break;
		}
	}

}

void Camera::applyTransform(void) {

	switch (_activeCameraMode.t)
	{
	case CAMERA_TYPE_FIXED: {

		float tx = _activeCameraMode.c.x * _worldScale * _activeCameraMode.z - (_displayProperties->viewportWidth / 2.0f);

		float ty = _activeCameraMode.c.y * _worldScale * _activeCameraMode.z - (_displayProperties->viewportHeight / 2.0f);

		glTranslatef(-tx, -ty, 0);

		this->applyEffectsTransforms();

		glScalef(_activeCameraMode.z, _activeCameraMode.z, 1);

		break;
							}
	case CAMERA_TYPE_FOLLOW_BALL:
	default:
		
		map<string, layoutItem> *items = _physics->getLayoutItems();
		
		layoutItem box;
		layoutItem lowBall;
		lowBall.width = -1;
		
		for (it_layoutItems iterator = items->begin(); iterator != items->end(); iterator++) {
			layoutItem item = iterator->second;
			if (strcmp("ball", item.o.n.c_str()) == 0) {
				if (lowBall.width == -1 || item.body->p.y < lowBall.body->p.y) {
					lowBall = item;
				}
			} else if (strcmp("box", item.n.c_str()) == 0) {
				box = item;
			}
		}

		GLfloat minY = box.v[0].y;
		GLfloat maxY = box.v[1].y;

		GLfloat posY = 0;

		posY = lowBall.body->p.y;
		posY -= lowBall.o.r1;

		posY -= _activeCameraMode.b.y; // margin

		if (posY < minY) {
			posY = minY;
		} else if (posY > maxY) {
			posY = maxY;
		}

		posY *= _worldScale * _activeCameraMode.z;

		glTranslatef(0, -posY, 0);

		glScalef(_activeCameraMode.z, _activeCameraMode.z, 1);
		
		break;
	}

	

}

void Camera::applyEffectsTransforms() {

	double curTime = absoluteTime();

	for (int i = 0; i < _activeEffects.size(); i++) {
		
		glRotatef(_activeEffects[i].aCurrent, 0, 0, 1);

		double delta = curTime - _activeEffects[i].startTime;

		_activeEffects[i].aCurrent = _activeEffects[i].aStart - (_activeEffects[i].aStart * delta / _activeEffects[i].d);

	}
	
}

void Camera::doEffect(const char *effectName) {

	cameraEffect effect = _effects[effectName];

	effect.aCurrent = effect.aStart;

	effect.startTime = absoluteTime();

	_activeEffects.push_back(effect);

}