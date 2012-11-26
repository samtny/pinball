
#include "GlutEngine.h"

#include <stdlib.h>
#include <GL/glut.h>

#include "Physics.h"

#include "Renderer.h"

#include "Game.h"

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

void GlutEngine::setGame(Game *game) {
	_game = game;
}

void glut_keyboardCallback(unsigned char key, int x, int y) {
	glut_currentInstance->keyboardCallback(key);
}

void glut_keyboardUpCallback(unsigned char key, int x, int y) {
	glut_currentInstance->keyboardUpCallback(key);
}

void GlutEngine::init() {
	
	//glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 50);
	glutCreateWindow("Porky Pinball");

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(glut_keyboardCallback);
	glutKeyboardUpFunc(glut_keyboardUpCallback);

	//_renderer->init();

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

void GlutEngine::keyboardCallback(unsigned char key) {
	
	if (key == 's') {
		// TODO: access _games public member "startButton" and pass to this method instead;
		_game->closeSwitch(0);
	} else if (key == '=') {
		// increase "zoom"
		_game->setZoomLevel(_game->getZoomLevel() + 0.25);
	} else if (key == '-') {
		// decrease "zoom"
		_game->setZoomLevel(_game->getZoomLevel() - 0.25);
	}

}

void GlutEngine::keyboardUpCallback(unsigned char key) {

}