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
	case EDIT_MODE_MOVE:
	case EDIT_MODE_MOVE_BEGIN:
	case EDIT_MODE_MOVE_COMMIT:
		moveItems();
		break;
	case EDIT_MODE_ROTATE:
	case EDIT_MODE_ROTATE_COMMIT:
		rotateItems();
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

void Editor::moveItems() {

	if (
		_state.editMode == EDIT_MODE_MOVE_COMMIT &&
		(_state.selectionStart.x != _state.selectionEnd.x  ||
		_state.selectionStart.x != _state.selectionEnd.y)
		) 
	{

		map<string, layoutItem> *items = _physics->getLayoutItems();

		Coord2 start = _camera->transform(_state.selectionStart);
		Coord2 end = _camera->transform(_state.selectionEnd);

		for (it_layoutItems it = items->begin(); it != items->end(); it++) {
	
			layoutItem *item = &(&*it)->second;

			if (item->editing == true) {

				_physics->destroyObject(item);

				for (int i = 0; i < item->count; i++) {
					item->v[i].x = item->v[i].x - (start.x - end.x);
					item->v[i].y = item->v[i].y - (start.y - end.y);
				}

				_physics->createObject(item);

			}

		}

	}

}

void Editor::rotateItems() {
	
	if (
		_state.editMode == EDIT_MODE_ROTATE_COMMIT &&
		_state.selectionStart.x != _state.selectionEnd.x
		) 
	{

		map<string, layoutItem> *items = _physics->getLayoutItems();

		Coord2 start = _state.selectionStart;
		Coord2 end = _state.selectionEnd;
		float rot = 360 * (((int)(start.x - end.x) % 100) / 100.0f);
		Coord2 rotvect = { cos(rot * M_PI / 180.0f), sin(rot * M_PI / 180.0f) };

		for (it_layoutItems it = items->begin(); it != items->end(); it++) {
			
			layoutItem *item = &(&*it)->second;

			if (item->editing == true) {

				_physics->destroyObject(item);

				// find center
				Coord2 c = { 0, 0 };
				for (int i = 0; i < item->count; i++) {
					Coord2 t = { item->v[i].x, item->v[i].y };
					c = coordadd(c, t);
				}
				c = coordmult(c, 1 / (float)item->count);
				
				// rotate all points around c
				for (int i = 0; i < item->count; i++) {
					
					// current vertex
					Coord2 v = { item->v[i].x, item->v[i].y };

					// relative to center
					Coord2 vrel = coordsub(v, c);

					// transform by rotvect
					Coord2 vt = {vrel.x * rotvect.x - vrel.y * rotvect.y, vrel.x * rotvect.y + vrel.y * rotvect.x};

					// add back center
					Coord2 vfinal = coordadd(c, vt);

					item->v[i].x = vfinal.x;
					item->v[i].y = vfinal.y;

				}

				_physics->createObject(item);

			}

		}

	}
	
}

