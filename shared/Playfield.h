
using namespace std;
#include <string>
#include <map>

class PinballBridgeInterface;
struct Material;
struct Part;
struct LayoutItem;

class Playfield {
public:
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void init(void);
protected:
	void loadConfig(void);
	void loadMaterials(void);
	void loadParts(void);
	void loadLayout(void);
private:
	PinballBridgeInterface *_bridgeInterface;
	map<string, Material> _materials;
	map<string, Part> _parts;
	map<string, LayoutItem> _layout;
};

