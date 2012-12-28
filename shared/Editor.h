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
	EDIT_MODE_MOVE_COMMIT,
	EDIT_MODE_ROTATE,
	EDIT_MODE_ROTATE_COMMIT,
	EDIT_MODE_INSERT,
	EDIT_MODE_INSERT_BEGIN,
	EDIT_MODE_DUPE
} EditMode;

typedef struct EditObject {
	objectProperties object;
	Coord2 verts[200];
	int vCurrent;
} EditObject;

typedef struct EditorState {
	EditMode editMode;
	Coord2 selectionStart;
	Coord2 selectionEnd;
	string editObjectName;
} EditorState;

class Editor {
public:
	Editor(void);
	~Editor(void);
	void init();
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void setGame(Game *game);
	void setPhysics(Physics *physics);
	void setCamera(Camera *camera);
	const EditorState *getState();
	const EditObject *getCurrentEditObject();
	void setState(EditorState state);
	void selectItems();
	void moveItems();
	void rotateItems();
	void insertItems();
	void dupeItems();
	void loadConfig();
	void loadMaterials();
	void loadObjects();
	vector<string> getObjectNames();
private:
	PinballBridgeInterface *_bridgeInterface;
	Game *_game;
	Physics *_physics;
	Camera *_camera;
	EditorState _state;
	map<string, materialProperties> _materials;
	map<string, objectProperties> _objects;
	EditObject _currentEditObject;
	int _currentEditObjectName;
};

