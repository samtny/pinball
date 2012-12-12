
typedef enum CameraType {
	CAMERA_TYPE_FOLLOW_BALL,
	CAMERA_TYPE_FIXED
} CameraType;

// TODO: move out;
#include "PinballBridgeInterface.h"
#include "Parts.h"

#include <string>
#include <vector>
#include <map>

class Renderer;
class Physics;

class Camera {
public:
	Camera();
	~Camera();
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void init();
	CameraType type;
	float minZoomLevel;
	float maxZoomLevel;
	void setDisplayProperties(HostProperties *displayProperties);
	void setPhysics(Physics *physics);
	void setZoomLevel(float zoomLevel);
	float getZoomLevel();
	void setMode(const char *modeName);
	Coord2 transform(Coord2 coord);
	void applyTransform(void);
	void doEffect(const char *effectName);
	void applyEffectsTransforms(void);
protected:
	void loadConfig();
	void loadCamera();
	void loadEffects();
private:
	PinballBridgeInterface *_bridgeInterface;
	HostProperties *_displayProperties;
	Physics *_physics;
	float _scale;
	void initModes();
	map<string, cameraEffect> _effects;
	vector<cameraEffect> _activeEffects;
};

