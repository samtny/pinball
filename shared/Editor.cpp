#include "Editor.h"

#include "chipmunk/chipmunk.h"

#include "PinballBridgeInterface.h"

#include "Game.h"

#include "Physics.h"

#include "Camera.h"

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

static void edit(cpShape *shape, void *data) {

	layoutItem *item = (layoutItem *)shape->data;
	cpBody *b = cpShapeGetBody(shape);
	if (!item) {
		item = (layoutItem *)b->data;
	}

	if (item) {
		
		EditParams *params = (EditParams *)data;

		if (params->editMode == EDIT_MODE_SELECT || params->editMode == EDIT_MODE_SELECT_MANY || params->editMode == EDIT_MODE_SELECT_EXCLUSIVE) {
			if (item->v[0].x > min(params->selectionStart.x, params->selectionEnd.x) && item->v[0].x < max(params->selectionStart.x, params->selectionEnd.x)
				&& item->v[0].y > min(params->selectionStart.y, params->selectionEnd.y) && item->v[0].y < max(params->selectionStart.y, params->selectionEnd.y)) {
				fprintf(stderr, "%s\n", item->n.c_str());
				item->editing = true;
			} else if (params->editMode == EDIT_MODE_SELECT_EXCLUSIVE) {
				item->editing = false;
			}
		} else if (params->editMode == EDIT_MODE_MOVE) {
			if (item->editing == true && b) {
				cpBodyResetForces(b);
				cpBodySetPos(b, cpv(params->selectionEnd.x, params->selectionEnd.y));
			}
		}

	}

}

void Editor::edit(EditParams params) {

	params.selectionStart = _camera->transform(params.selectionStart);
	params.selectionEnd = _camera->transform(params.selectionEnd);

	cpSpace *space = _physics->getSpace();

	cpSpaceEachShape(space, &::edit, &params);

}

