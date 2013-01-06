
#include <string>
#include <map>

class PinballBridgeInterface;

#include "Parts.h"

class Playfield {
public:
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void init(void);
	std::map<std::string, Material> *getMaterials();
	std::map<std::string, Texture> *getTextures();
	std::map<std::string, Overlay> *getOverlays();
	std::map<std::string, LayoutItem> *getLayout();
protected:
	void loadConfig(void);
	void loadMaterials(void);
	void loadTextures(void);
	void loadOverlays(void);
	void loadParts(void);
	void loadLayout(void);
private:
	PinballBridgeInterface *_bridgeInterface;
	std::map<std::string, Material> _materials;
	std::map<std::string, Texture> _textures;
	std::map<std::string, Overlay> _overlays;
	std::map<std::string, Part> _parts;
	std::map<std::string, LayoutItem> _layout;
	double _scale;
};

