#include "Parts.h"

class PinballBridgeInterface;
class Game;
class Physics;
class Camera;

typedef enum EditMode {
	EDIT_MODE_SELECT,
	EDIT_MODE_SELECT_EXCLUSIVE,
	EDIT_MODE_SELECT_MANY,
	EDIT_MODE_MOVE
} EditMode;

typedef struct EditParams {
	Coord2 selectionStart;
	Coord2 selectionEnd;
	EditMode editMode;
} EditParams;

class Editor {
public:
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void setGame(Game *game);
	void setPhysics(Physics *physics);
	void setCamera(Camera *camera);
	void edit(EditParams params);
private:
	PinballBridgeInterface *_bridgeInterface;
	Game *_game;
	Physics *_physics;
	Camera *_camera;
};

