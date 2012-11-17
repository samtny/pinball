#include "Camera.h"

#include "Renderer.h"

#include "Physics.h"

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

Camera::Camera() : mode(CAMERA_MODE_FOLLOW_BALL), minY(0), maxY(1), minZoomLevel(1), maxZoomLevel(1), zoomLevel(1), margin(1)
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

void Camera::applyTransform(void) {


	switch (mode)
	{
	case CAMERA_MODE_FOLLOW_BALL:
	default:
		/*
		GLfloat posY = _physics->getBallSlerped()->p.y; // ball center
		posY -= _physics->layoutItems["ball"].o.r1; // full ball
		posY -= _camera->margin; // margin

		if (posY < _camera->minY) {
			posY = _camera->minY;
		} else if (posY > _camera->maxY * _camera->zoomLevel) {
			posY = _camera->maxY * _camera->zoomLevel;
		}
		posY *= _scale * _camera->zoomLevel;

		glTranslatef(0, -posY, 0);

		glScalef(_camera->zoomLevel, _camera->zoomLevel, 0);
		*/
		break;
	}


}