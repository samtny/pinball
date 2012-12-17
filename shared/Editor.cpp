#include "Editor.h"

#include "chipmunk/chipmunk.h"

#include "PinballBridgeInterface.h"

#include "Game.h"

#include "Physics.h"

#include "Camera.h"

static Editor *_editorCurrentInstance;

Editor::Editor(void) {
	_editorCurrentInstance = this;
	_state.editMode = EDIT_MODE_NONE;
	_state.selectionStart.x = 0;
	_state.selectionStart.y = 0;
	_state.selectionEnd.x = 0;
	_state.selectionEnd.y = 0;
}

Editor::~Editor(void) {

}

void Editor::setBridgeInterface(PinballBridgeInterface *bridgeInterface) {
	_bridgeInterface = bridgeInterface;
}

void Editor::setGame(Game *game) {
	_game = game;
}

void Editor::setPhysics(Physics *physics) {
	_physics = physics;
}

void Editor::setCamera(Camera *camera) {
	_camera = camera;
}

const EditorState *Editor::getState() {
	return &_state;
}

void Editor::setState(EditorState state) {

	_state = state;

	switch (_state.editMode) {
	case EDIT_MODE_SELECT:
	case EDIT_MODE_SELECT_EXCLUSIVE:
	case EDIT_MODE_SELECT_MANY:
		selectItems();
		break;
	default:
		break;
	}
	
}


void Editor::selectItems() {

	map<string, layoutItem> *items = _physics->getLayoutItems();

	Coord2 start = _camera->transform(_state.selectionStart);
	Coord2 end = _camera->transform(_state.selectionEnd);

	for (it_layoutItems it = items->begin(); it != items->end(); it++) {
	
		layoutItem *item = &(&*it)->second;

		
		bool inside = false;

		for (int i = 0; i < item->count; i++) {
			cpVect v = item->v[i];
			if (
				v.x >= min(start.x, end.x) &&
				v.x <= max(start.x, end.x) &&
				v.y >= min(start.y, end.y) &&
				v.y <= max(start.y, end.y)
				) {
				inside = true;
				break;
			}
		}

		if (inside == true) {
			item->editing = true;
		} else if (_state.editMode == EDIT_MODE_SELECT_EXCLUSIVE) {
			item->editing = false;
		}

	}

}





