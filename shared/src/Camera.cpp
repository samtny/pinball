#include "Camera.h"

#include "PinballBridgeInterface.h"

#include "Types.h"

#include "Playfield.h"

#include "Parts.h"

#include "Util.h"

extern "C" {
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}

#ifdef __APPLE__
	#include <OpenGLES/ES1/gl.h>
	#include <OpenGLES/ES1/glext.h>
#else
	#ifdef _WIN32
		#include <windows.h>
		#include <GL/GL.h>
		#include <GL/GLU.h>
	#else
		#include <GL/gl.h>
		#include <GL/glu.h>
	#endif
#endif

#ifndef FLT_MAX
	#include <values.h>
#endif

using std::string;
using std::map;

Camera::Camera()
{
	_pan = coord(0, 0);
}

Camera::~Camera() {
}

void Camera::setBridgeInterface(PinballBridgeInterface *bridgeInterface) {
	_bridgeInterface = bridgeInterface;
}

void Camera::init() {
	
	_displayProperties = _bridgeInterface->getHostProperties();

	this->loadConfig();
	this->loadCamera();
	this->loadEffects();

	for (it_CameraMode iterator = _cameraModes.begin(); iterator != _cameraModes.end(); iterator++) {
		CameraMode *mode = &(&*iterator)->second;
		//mode->b.x *= 1 / _scale;
		//mode->b.y *= 1 / _scale;
		mode->c.x *= 1 / _scale;
		mode->c.y *= 1 / _scale;
		mode->w *= 1 / _scale;
	}

}

void Camera::loadConfig() {

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
                    
				if (strcmp("scale", key) == 0) {

					_scale = (float)lua_tonumber(L, -1);

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

	const char *configFileName = _bridgeInterface->getScriptPath((const char *)"camera.lua");

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

					} else if (strcmp(key, "w") == 0) {

						mode.w = (float)lua_tonumber(L, -1);

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

	const char *effectsFileName = _bridgeInterface->getScriptPath((const char *)"effects.lua");

	int error = luaL_dofile(L, effectsFileName);
	if (!error) {

        lua_getglobal(L, "effects");

		if (lua_istable(L, -1)) {

			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				const char *name = lua_tostring(L, -2);
                    
				CameraEffect effect;
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

void Camera::setPlayfield(Playfield *playfield) {
	_playfield = playfield;
}

void Camera::setZoomLevel(float zoomLevel) {
	if (_activeCameraMode.z <= _maxZoomLevel && zoomLevel >= _minZoomLevel) {
		_activeCameraMode.z = zoomLevel;
	}
}

float Camera::getZoomLevel() {
	return _activeCameraMode.z;
}

void Camera::setMode(const char *modeName) {

	for (it_CameraMode iterator = _cameraModes.begin(); iterator != _cameraModes.end(); iterator++) {
		CameraMode mode = iterator->second;
		if (strcmp(modeName, mode.name.c_str()) == 0) {
			_activeCameraMode = mode;
			break;
		}
	}

}

Coord2 Camera::transform(Coord2 coord) {

	float _activeCameraModeH = _activeCameraMode.w / _displayProperties->viewportWidth * _displayProperties->viewportHeight;

	cpFloat tx = coord.x * 1 / _scale + _activeCameraMode.c.x - (_activeCameraMode.w / 2.0f);
	cpFloat ty = _activeCameraModeH - coord.y * 1 / _scale + _activeCameraMode.c.y - _activeCameraModeH / 2.0f;

	Coord2 transformed = {tx + _pan.x * 1 / _scale, ty - _pan.y * 1 / _scale};

	return transformed;

}

Coord2 Camera::scale(Coord2 coord) {

	_scale = (_displayProperties->viewportWidth / _activeCameraMode.w) * _activeCameraMode.z;

	return coordmult(coord, 1 / _scale);

}

void Camera::applyTransform(void) {

	_scale = (_displayProperties->viewportWidth / _activeCameraMode.w) * _activeCameraMode.z;

	switch (_activeCameraMode.t)
	{
		case CAMERA_TYPE_FIXED: {

			float tx = (float)(_activeCameraMode.c.x * _scale - (_displayProperties->viewportWidth / 2.0f));

			float ty = (float)(_activeCameraMode.c.y * _scale - (_displayProperties->viewportHeight / 2.0f));

			glTranslatef(-tx + -(float)_pan.x, -ty + (float)_pan.y, 0);

			//this->applyEffectsTransforms();

			glScalef(_scale, _scale, 1);

			break;
		}
		case CAMERA_TYPE_FOLLOW_BALL:
			// fall thru...
		default:

			// optimize for low ball.y + avg balls.x
			float ballX = 0;
			float ballY = FLT_MAX;
			float ballRadius = 0;
			float boxLeftX = FLT_MAX;
			float boxRightX = FLT_MIN;
			float boxBottomY = FLT_MAX;
			float boxTopY = FLT_MIN;
			
			int numBalls = 0;
			for (it_LayoutItem it = _playfield->getLayout()->begin(); it != _playfield->getLayout()->end(); it++) {

				//LayoutItem item = it->second;
				LayoutItem *item = &(&*it)->second;

				if (strcmp(item->o->s.c_str(), "ball") == 0) {

					ballX += (float)item->bodies[0]->p.x;
					numBalls++;

					float y = (float)item->bodies[0]->p.y;

					if (y < ballY) {
						ballY = y;
						ballRadius = item->o->r1 * item->s;
					}

				} else if (strcmp(item->o->s.c_str(), "box") == 0) {

					boxLeftX = (float)item->v[0].x;
					boxRightX = (float)item->v[3].x;

					boxBottomY = (float)item->v[0].y;
					boxTopY = (float)item->v[1].y;
					
				}

			}
			
			ballX *= 1.0f / (float)numBalls;

			ballX *= _scale;
			ballY *= _scale;
			ballRadius *= _scale;
			boxLeftX *= _scale;
			boxRightX *= _scale;
			boxBottomY *= _scale;
			boxTopY *= _scale;

			float viewableX = (float)_displayProperties->viewportWidth;
			float bufferX = (float)_activeCameraMode.b.x * viewableX;

			float tx = ballX - bufferX;

			if (tx < (boxLeftX)) {
				tx = boxLeftX;
			}

			if ((tx + viewableX) > boxRightX) {
				tx = boxRightX - viewableX;
			}

			float viewableY = (float)_displayProperties->viewportHeight;
			float bufferY = (float)_activeCameraMode.b.y * viewableY;

			float ty = boxBottomY;

			float bufferedY = ballY + ballRadius + bufferY;

			if (bufferedY > (boxBottomY + viewableY)) {
				ty += bufferedY - (boxBottomY + viewableY);
			}

			if ((ty + viewableY) > boxTopY) {
				ty = boxTopY - viewableY;
			}

			glTranslatef(-tx + -(float)_pan.x, -ty + (float)_pan.y, 0);
			glScalef(_scale, _scale, 1);

			break;

	}

}

void Camera::applyEffectsTransforms() {

	double curTime = absoluteTime();

	for (int i = 0; i < (int)_activeEffects.size(); i++) {
		
		glRotatef((float)_activeEffects[i].aCurrent, 0, 0, 1);

		double delta = curTime - _activeEffects[i].startTime;

		_activeEffects[i].aCurrent = _activeEffects[i].aStart - (_activeEffects[i].aStart * delta / _activeEffects[i].d);

	}
	
}

void Camera::doEffect(const char *effectName) {

	CameraEffect effect = _effects[effectName];

	effect.aCurrent = effect.aStart;

	effect.startTime = absoluteTime();

	_activeEffects.push_back(effect);

}

void Camera::setPan(Coord2 pan) {
	_pan = pan;
}

Coord2 Camera::getPan() {
	return _pan;
}

