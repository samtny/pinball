#include "Camera.h"

#include "Renderer.h"

#include "Physics.h"

#include "Parts.h"

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

typedef struct Point{float x,y;} Point;

struct CameraMode {
	string name;
	CameraType t;
	Point p;
};

Camera::Camera() : type(CAMERA_TYPE_FOLLOW_BALL), _zoomLevel(1), maxZoomLevel(1), minZoomLevel(1)
{

}

Camera::~Camera() {
}

void Camera::setBridgeInterface(PinballBridgeInterface *bridgeInterface) {
	_bridgeInterface = bridgeInterface;
}

void Camera::init() {
	
	this->loadConfig);

}

void Camera::loadConfig() {

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *configFileName = _bridgeInterface->getPathForScriptFileName((void *)"camera.lua");

	int error = luaL_dofile(L, configFileName);
	if (!error) {

        lua_getglobal(L, "camera");

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

void Camera::setDisplayProperties(HostProperties *displayProperties) {
	_displayProperties = displayProperties;
}

void Camera::setRenderer(Renderer *renderer) {
	_renderer = renderer;
}

void Camera::setPhysics(Physics *physics) {
	_physics = physics;
}

void Camera::setZoomLevel(float zoomLevel) {
	if (zoomLevel <= maxZoomLevel && zoomLevel >= minZoomLevel) {
		_zoomLevel = zoomLevel;

	}
}

float Camera::getZoomLevel() {
	return _zoomLevel;
}

void Camera::setWorldScale(float worldScale) {
	_worldScale = worldScale;
}

void Camera::setModeFollowBall() {

	mode = CAMERA_MODE_FOLLOW_BALL;
	
	map<string, layoutItem> *items = _physics->getLayoutItems();
		
	layoutItem box;
		
	for (it_layoutItems iterator = items->begin(); iterator != items->end(); iterator++) {
		layoutItem item = iterator->second;
		if (strcmp("box", item.n.c_str()) == 0) {
			box = item;
		}
	}

	minY = box.v[0].y;
	
	// total height - (total height - visible height)
	float diff = box.height - (box.height - _displayProperties->viewportHeight / _worldScale);

	maxY = box.v[1].y;
	//maxY = diff;

	marginY = (box.v[1].y - box.v[0].y) * 0.15;

}

void Camera::applyTransform(void) {

	switch (mode)
	{
	case CAMERA_MODE_FOLLOW_BALL:
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

		GLfloat posY = 0;

		posY = lowBall.body->p.y;
		posY -= lowBall.o.r1;

		posY -= marginY; // margin

		if (posY < minY) {
			posY = minY;
		} else if (posY > maxY) {
			posY = maxY;
		}

		posY *= _worldScale * _zoomLevel;

		glTranslatef(0, -posY, 0);

		glScalef(_zoomLevel, _zoomLevel, 0);
		
		break;
	}

}