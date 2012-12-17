
#include "GlutEngine.h"

#include <stdlib.h>
#include <GL/glut.h>

#include "Physics.h"

#include "Renderer.h"

#include "Game.h"

#include "Editor.h"

#include "Parts.h"

GlutEngine *glut_currentInstance;

static Coord2 selectionStart = { 0, 0 };
static Coord2 selectionEnd = { 0, 0 };

static EditMode _currentEditMode = EDIT_MODE_SELECT;

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

void GlutEngine::setEditor(Editor *editor) {
	_editor = editor;
}

void glut_keyboardCallback(unsigned char key, int x, int y) {
	glut_currentInstance->keyboardCallback(key);
}

void glut_keyboardUpCallback(unsigned char key, int x, int y) {
	glut_currentInstance->keyboardUpCallback(key);
}

void glut_mouseCallback (int button, int state, int x, int y) {

	fprintf(stderr, "%s\n", "glut_mouseCallback");
	glut_currentInstance->mouseCallback(button, state, x, y);

}

void glut_menuFunc(int value) {
	glut_currentInstance->menuCallback(value);
}

typedef enum Menu {
	MENU_SELECT,
	MENU_MOVE,
	MENU_ROTATE
} Menu;

void GlutEngine::menuCallback(int value) {

	switch (value)
	{
	case MENU_SELECT:
		_currentEditMode = EDIT_MODE_SELECT;
		break;
	case MENU_MOVE:
		_currentEditMode = EDIT_MODE_MOVE;
		break;
	case MENU_ROTATE:
		_currentEditMode = EDIT_MODE_ROTATE;
	default:
		break;
	}

}

void glut_motionCallback(int x, int y) {
	glut_currentInstance->motionCallback(x, y);
}

void GlutEngine::init() {
	
	//glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 50);
	glutCreateWindow("");

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(glut_keyboardCallback);
	glutKeyboardUpFunc(glut_keyboardUpCallback);
	//glutSpecialFunc(

	glutMouseFunc(glut_mouseCallback);
	glutMotionFunc(glut_motionCallback);

	//_renderer->init();

	glutCreateMenu(glut_menuFunc);
	glutAddMenuEntry("Select", MENU_SELECT);
	glutAddMenuEntry("Move", MENU_MOVE);
	glutAddMenuEntry("Rotate", MENU_ROTATE);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

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

	glutSetWindowTitle(_game->getGameName());

	glutTimerFunc(SLEEP_TICKS, glut_timerFunc, 0);
	glutDisplayFunc(glut_displayFunc);
	glutMainLoop();

}

void GlutEngine::timerFunc(int value) {

	glutTimerFunc(SLEEP_TICKS, glut_timerFunc, 0);
	
	if  (!_game->getPaused()) {
		this->_physics->updatePhysics();
	}

	glutPostRedisplay();
	
}

void GlutEngine::displayFunc() {

	
	static int time = 0;
	static int frame = 0;
	static int timebase = 0;

	frame++;
	time=glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		fprintf(stderr,"FPS:%4.2f",
			frame*1000.0/(time-timebase));
		timebase = time;
		frame = 0;
	}
	

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
	} else if (key == 'p') {
		_game->setPaused(!_game->getPaused());
	}

}

void GlutEngine::keyboardUpCallback(unsigned char key) {

}

void GlutEngine::motionCallback(int x, int y) {
	
	if (_currentEditMode == EDIT_MODE_SELECT) {
		const EditorState *s = _editor->getState();
		EditorState newState = { s->editMode, s->selectionStart, { x, y } };
		_editor->setState(newState);
	} else if (_currentEditMode == EDIT_MODE_MOVE) {
		const EditorState *s = _editor->getState();
		EditorState newState = { EDIT_MODE_MOVE, s->selectionStart, { x, y } };
		_editor->setState(newState);
	} else if (_currentEditMode == EDIT_MODE_ROTATE) {
		const EditorState *s = _editor->getState();
		EditorState newState = { EDIT_MODE_ROTATE, s->selectionStart, { x, y } };
		_editor->setState(newState);
	}

}

void GlutEngine::mouseCallback(int button, int state, int x, int y) {

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		switch (state)
		{
		case GLUT_DOWN:
			if (_currentEditMode == EDIT_MODE_SELECT) {
				const EditorState *s = _editor->getState();
				EditorState newState = { glutGetModifiers() == GLUT_ACTIVE_SHIFT ? EDIT_MODE_SELECT_MANY : EDIT_MODE_SELECT_EXCLUSIVE, { x, y }, { x, y } };
				_editor->setState(newState);
			} else if (_currentEditMode == EDIT_MODE_MOVE) {
				const EditorState *s = _editor->getState();
				EditorState newState = { EDIT_MODE_MOVE_BEGIN, { x, y }, { x, y } };
				_editor->setState(newState);
			} else if (_currentEditMode == EDIT_MODE_ROTATE) {
				const EditorState *s = _editor->getState();
				EditorState newState = { EDIT_MODE_ROTATE, { x, y }, { x, y } };
				_editor->setState(newState);
			}
			break;
		case GLUT_UP: {
			if (_currentEditMode == EDIT_MODE_SELECT) {
				// do nothing
			} else if (_currentEditMode == EDIT_MODE_MOVE) {
				const EditorState *s = _editor->getState();
				EditorState newState = { EDIT_MODE_MOVE_COMMIT, s->selectionStart, s->selectionEnd };
				_editor->setState(newState);
			} else if (_currentEditMode == EDIT_MODE_ROTATE) {
				const EditorState *s = _editor->getState();
				EditorState newState = { EDIT_MODE_ROTATE_COMMIT, s->selectionStart, s->selectionEnd };
				_editor->setState(newState);
			}
			break;
			}
		default:
			break;
		}
		break;
	default:
		break;
	}

}


