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
	if (!item) {
		cpBody *b = cpShapeGetBody(shape);
		item = (layoutItem *)b->data;
	}

	if (item) {
			
		EditParams *params = (EditParams *)data;

		if (item->v[0].x > min(params->selectRect.begin.x, params->selectRect.end.x) && item->v[0].x < max(params->selectRect.begin.x, params->selectRect.end.x)
			&& item->v[0].y > min(params->selectRect.begin.y, params->selectRect.end.y) && item->v[0].y < max(params->selectRect.begin.y, params->selectRect.end.y)) {
			fprintf(stderr, "%s\n", item->n.c_str());
			item->editing = true;
		} else if (params->editMode == EDIT_MODE_SELECT_EXCLUSIVE) {
			item->editing = false;
		}

	}

}

void Editor::edit(EditParams params) {

	params.selectRect.begin = _camera->transform(params.selectRect.begin);
	params.selectRect.end = _camera->transform(params.selectRect.end);

	cpSpace *space = _physics->getSpace();

	cpSpaceEachShape(space, &::edit, &params);

}

