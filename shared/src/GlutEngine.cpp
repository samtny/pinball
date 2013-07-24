
#include "GlutEngine.h"

#include <stdlib.h>
#include <GL/glut.h>

#include "Physics.h"

#include "Renderer.h"

#include "Game.h"

#include "Editor.h"

#include "Parts.h"

using std::string;
using std::vector;

GlutEngine *glut_currentInstance;

//static Coord2 selectionStart = { 0, 0 };
//static Coord2 selectionEnd = { 0, 0 };

static EditMode _currentEditMode = EDIT_MODE_SELECT;

static int _menuMain;
static int _menuInsert;

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

	this->updateMenuEditorItems();

}

void glut_keyboardCallback(unsigned char key, int x, int y) {
	glut_currentInstance->keyboardCallback(key);
}

void glut_keyboardUpCallback(unsigned char key, int x, int y) {
	glut_currentInstance->keyboardUpCallback(key);
}

void glut_mouseCallback (int button, int state, int x, int y) {

	glut_currentInstance->mouseCallback(button, state, x, y);

}

void glut_menuFunc(int value) {
	glut_currentInstance->menuCallback(value);
}

void glut_menuFuncInsert(int value) {
	glut_currentInstance->menuCallbackInsert(value);
}

typedef enum Menu {
	MENU_NONE,
	MENU_PAN,
	MENU_SELECT,
	MENU_MOVE,
	MENU_ROTATE,
	MENU_DUPE,
	MENU_SAVE,
	MENU_LOAD
} Menu;

void GlutEngine::menuCallback(int value) {

	switch (value)
	{
	case MENU_PAN:
		_currentEditMode = EDIT_MODE_PAN;
		break;
	case MENU_SELECT:
		_currentEditMode = EDIT_MODE_SELECT;
		break;
	case MENU_MOVE:
		_currentEditMode = EDIT_MODE_MOVE;
		break;
	case MENU_ROTATE:
		_currentEditMode = EDIT_MODE_ROTATE;
		break;
	case MENU_DUPE: {
			const EditorState *s = _editor->getState();
			EditorState newState = { EDIT_MODE_DUPE, s->selectionStart, s->selectionEnd };
			_editor->setState(newState);
			_currentEditMode = EDIT_MODE_MOVE;
		}
		break;
	case MENU_SAVE:
		_editor->save();
		break;
	case MENU_LOAD:
		_editor->load();
		break;
	default:
		break;
	}

}

void GlutEngine::menuCallbackInsert(int index) {

	vector<string> names = _editor->getObjectNames();
	
	const EditorState *s = _editor->getState();
	EditorState newState = { EDIT_MODE_INSERT_BEGIN, s->selectionStart, s->selectionEnd, names[index] };
	_editor->setState(newState);

	_currentEditMode = EDIT_MODE_INSERT;

}

void glut_motionCallback(int x, int y) {
	glut_currentInstance->motionCallback(x, y);
}

void GlutEngine::init() {
	
	#ifdef __linux__
	char *argv[1];
	int argc=1;
	glutInit(&argc, argv);
	#endif
	
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(1200, 800);
	glutInitWindowPosition(100, 20);
	glutCreateWindow("");

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(glut_keyboardCallback);
	glutKeyboardUpFunc(glut_keyboardUpCallback);
	//glutSpecialFunc(

	glutMouseFunc(glut_mouseCallback);
	glutMotionFunc(glut_motionCallback);

	//_renderer->init();

	// insert menu
	glutCreateMenu(glut_menuFuncInsert);
	//glutAddMenuEntry("__yep", NULL);
	_menuInsert = glutGetMenu();

	// main menu
	glutCreateMenu(glut_menuFunc);
	glutAddMenuEntry("Pan", MENU_PAN);
	glutAddMenuEntry("Select", MENU_SELECT);
	glutAddMenuEntry("Move", MENU_MOVE);
	glutAddMenuEntry("Rotate", MENU_ROTATE);
	glutAddMenuEntry("Dupe", MENU_DUPE);
	glutAddSubMenu("New", _menuInsert);
	glutAddMenuEntry("Save", MENU_SAVE);
	glutAddMenuEntry("Load", MENU_LOAD);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	_menuMain = glutGetMenu();

}

void GlutEngine::updateMenuEditorItems() {

	glutSetMenu(_menuInsert);

	vector<string> names = _editor->getObjectNames();

	for (int i = 0; i < (int)names.size(); i++) {
		glutAddMenuEntry(names[i].c_str(), i);
	}

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
	} else if (key == '/') {
		_game->switchClosed("rbutton", NULL);
	} else if (key == 'z') {
		_game->switchClosed("lbutton", NULL);
	} else if (key == 26) {
		_editor->undo();
	} else if (key == 127) {
		_editor->deleteItems();
	} else if (key == 'x') {
		_game->nudge( { (cpFloat)1.0, (cpFloat)0.0 });
	} else if (key == '.') {
		_game->nudge( { (cpFloat)-1.0, (cpFloat)0.0 });
	}

}

void GlutEngine::keyboardUpCallback(unsigned char key) {

	if (key == '/') {
		_game->switchOpened("rbutton", NULL);
	} else if (key == 'z') {
		_game->switchOpened("lbutton", NULL);
	}

}

void GlutEngine::motionCallback(int x, int y) {
	
	if (_currentEditMode == EDIT_MODE_PAN) {
		const EditorState *s = _editor->getState();
		EditorState newState = { s->editMode, s->selectionStart, { (cpFloat)x, (cpFloat)y } };
		_editor->setState(newState);
	} else if (_currentEditMode == EDIT_MODE_SELECT) {
		const EditorState *s = _editor->getState();
		EditorState newState = { s->editMode, s->selectionStart, { (cpFloat)x, (cpFloat)y } };
		_editor->setState(newState);
	} else if (_currentEditMode == EDIT_MODE_MOVE) {
		const EditorState *s = _editor->getState();
		EditorState newState = { EDIT_MODE_MOVE, s->selectionStart, { (cpFloat)x, (cpFloat)y } };
		_editor->setState(newState);
	} else if (_currentEditMode == EDIT_MODE_ROTATE) {
		const EditorState *s = _editor->getState();
		EditorState newState = { EDIT_MODE_ROTATE, s->selectionStart, { (cpFloat)x, (cpFloat)y } };
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
			if (_currentEditMode == EDIT_MODE_PAN) {
				//const EditorState *s = _editor->getState();
				EditorState newState = { EDIT_MODE_PAN, { (cpFloat)x, (cpFloat)y }, { (cpFloat)x, (cpFloat)y } };
				_editor->setState(newState);
			} else if (_currentEditMode == EDIT_MODE_SELECT) {
				//const EditorState *s = _editor->getState();
				EditorState newState = { glutGetModifiers() == GLUT_ACTIVE_SHIFT ? EDIT_MODE_SELECT_MANY : EDIT_MODE_SELECT_EXCLUSIVE, { (cpFloat)x, (cpFloat)y }, { (cpFloat)x, (cpFloat)y } };
				_editor->setState(newState);
			} else if (_currentEditMode == EDIT_MODE_MOVE) {
				//const EditorState *s = _editor->getState();
				EditorState newState = { EDIT_MODE_MOVE_BEGIN, { (cpFloat)x, (cpFloat)y }, { (cpFloat)x, (cpFloat)y } };
				_editor->setState(newState);
			} else if (_currentEditMode == EDIT_MODE_ROTATE) {
				//const EditorState *s = _editor->getState();
				EditorState newState = { EDIT_MODE_ROTATE, { (cpFloat)x, (cpFloat)y }, { (cpFloat)x, (cpFloat)y } };
				_editor->setState(newState);
			} else if (_currentEditMode == EDIT_MODE_INSERT) {
				//const EditorState *s = _editor->getState();
				EditorState newState = { EDIT_MODE_INSERT, { (cpFloat)x, (cpFloat)y }, { (cpFloat)x, (cpFloat)y } };
				_editor->setState(newState);
			}
			break;
		case GLUT_UP: {
			if (_currentEditMode == EDIT_MODE_PAN) {
				const EditorState *s = _editor->getState();
				EditorState newState = { EDIT_MODE_PAN_COMMIT, s->selectionStart, s->selectionEnd };
				_editor->setState(newState);
			} else if (_currentEditMode == EDIT_MODE_SELECT) {
				// do nothing
			} else if (_currentEditMode == EDIT_MODE_MOVE) {
				const EditorState *s = _editor->getState();
				EditorState newState = { EDIT_MODE_MOVE_COMMIT, s->selectionStart, s->selectionEnd };
				_editor->setState(newState);
			} else if (_currentEditMode == EDIT_MODE_ROTATE) {
				const EditorState *s = _editor->getState();
				EditorState newState = { EDIT_MODE_ROTATE_COMMIT, s->selectionStart, s->selectionEnd };
				_editor->setState(newState);
			} else if (_currentEditMode == EDIT_MODE_INSERT) {
				_editor->insertItems();
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


