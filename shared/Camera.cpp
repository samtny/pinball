#include "Camera.h"

#include "Renderer.h"

#include "Physics.h"

#include "Parts.h"

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

Camera::Camera() : mode(CAMERA_MODE_FOLLOW_BALL), _zoomLevel(1), maxZoomLevel(3), minZoomLevel(0.25)
{

}

Camera::~Camera() {
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
	maxY = box.v[1].y;
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
		} else if (posY > maxY * _zoomLevel) {
			posY = maxY * _zoomLevel;
		}
		posY *= _worldScale * _zoomLevel;

		glTranslatef(0, -posY, 0);

		glScalef(_zoomLevel, _zoomLevel, 0);
		
		break;
	}

}