
using namespace std;
#include <string>
#include <map>

class PinballBridgeInterface;

#include "Parts.h"

class Playfield {
public:
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void init(void);
	map<string, Material> *getMaterials();
	map<string, LayoutItem> *getLayout();
protected:
	void loadConfig(void);
	void loadMaterials(void);
	void loadTextures(void);
	void loadParts(void);
	void loadLayout(void);
private:
	PinballBridgeInterface *_bridgeInterface;
	map<string, Material> _materials;
	map<string, Texture> _textures;
	map<string, Part> _parts;
	map<string, LayoutItem> _layout;
};

