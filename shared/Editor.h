
#include <string>
#include <vector>
#include <map>

class PinballBridgeInterface;

class Playfield;
class Game;
class Physics;
class Camera;
struct Material;
struct Texture;
struct Part;
struct LayoutItem;
struct Coord2;

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
	Part *object;
	vector<Coord2> verts;
	int vCurrent;
} EditObject;

// TODO: editor should not accept state obj in
#include "Types.h"

typedef struct EditorState {
	EditMode editMode;
	Coord2 selectionStart;
	Coord2 selectionEnd;
	string editObjectName;
	map<string, LayoutItem> items;
} EditorState;

class Editor {
public:
	Editor(void);
	~Editor(void);
	void init();
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void setPlayfield(Playfield *playfield);
	void setGame(Game *game);
	void setPhysics(Physics *physics);
	void setCamera(Camera *camera);
	const EditorState *getState();
	const EditObject *getCurrentEditObject();
	void setState(EditorState state);
	void pushState();
	void selectItems();
	void moveItems();
	void rotateItems();
	void insertItems();
	void dupeItems();
	void deleteItems();
	void undo();
	void save();
	void load();
	void loadConfig();
	void loadMaterials();
	void loadTextures();
	void loadParts();
	void loadLayout();
	vector<string> getObjectNames();
private:
	PinballBridgeInterface *_bridgeInterface;
	Playfield *_playfield;
	Game *_game;
	Physics *_physics;
	Camera *_camera;
	EditorState _state;
	vector<EditorState> _history;
	map<string, Material> _materials;
	map<string, Texture> _textures;
	map<string, Part> _parts;
	map<string, LayoutItem> _layout;
	EditObject _currentEditObject;
	int _currentEditObjectName;
	double _scale;
};

