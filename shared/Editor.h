#include "Parts.h"

class PinballBridgeInterface;
class Game;
class Physics;
class Camera;

typedef enum EditMode {
	EDIT_MODE_NONE,
	EDIT_MODE_SELECT,
	EDIT_MODE_SELECT_EXCLUSIVE,
	EDIT_MODE_SELECT_MANY,
	EDIT_MODE_MOVE_BEGIN,
	EDIT_MODE_MOVE,
	EDIT_MODE_MOVE_COMMIT
} EditMode;

typedef struct EditorState {
	EditMode editMode;
	Coord2 selectionStart;
	Coord2 selectionEnd;
} EditorState;

class Editor {
public:
	Editor(void);
	~Editor(void);
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void setGame(Game *game);
	void setPhysics(Physics *physics);
	void setCamera(Camera *camera);
	const EditorState *getState();
	void setState(EditorState state);
	void selectItems();
private:
	PinballBridgeInterface *_bridgeInterface;
	Game *_game;
	Physics *_physics;
	Camera *_camera;
	EditorState _state;
};

