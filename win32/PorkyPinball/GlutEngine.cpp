
#include "GlutEngine.h"

#include <stdlib.h>
#include "glut.h"

#include "Physics.h"

#include "Renderer.h"

GlutEngine *glut_currentInstance;

GlutEngine::GlutEngine() {
	glut_currentInstance = this;
}

GlutEngine::~GlutEngine() {
}

void GlutEngine::setPhysics(Physics *physics) {
	_physics = physics;
}

void GlutEngine::setRenderer(Renderer *renderer) {
	_renderer = renderer;
}

void GlutEngine::init() {
	
	//glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 50);
	glutCreateWindow("Porky Pinball");

	glutIgnoreKeyRepeat(1);
	//glutKeyboardFunc(keyboardCallback);
	//glutKeyboardUpFunc(keyboardUpCallback);

	_renderer->init();

}


void glut_timerFunc(int value)
{
	glut_currentInstance->timerFunc(value);
}

void glut_displayFunc()
{
	glut_currentInstance->displayFunc();
}

void GlutEngine::start() {

	glutTimerFunc(SLEEP_TICKS, glut_timerFunc, 0);
	glutDisplayFunc(glut_displayFunc);
	glutMainLoop();

}

void GlutEngine::timerFunc(int value) {
	glutTimerFunc(SLEEP_TICKS, glut_timerFunc, 0);
	// TODO: for loop...
	this->_physics->updatePhysics();
	glutPostRedisplay();
}

void GlutEngine::displayFunc() {

	this->_renderer->draw();
	glutSwapBuffers();

}
