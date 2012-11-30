
typedef enum CameraType {
	CAMERA_TYPE_FOLLOW_BALL,
	CAMERA_TYPE_FIXED
} CameraType;

// TODO: move out;
#include "PinballBridgeInterface.h"
#include "Parts.h"

#include <string>
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
	void setWorldScale(float worldScale);
	void setMode(const char *modeName);
	void applyTransform(void);
	void doEffect(const char *effectName);
protected:
	void loadConfig();
	void loadCamera();
private:
	PinballBridgeInterface *_bridgeInterface;
	HostProperties *_displayProperties;
	Physics *_physics;
	float _worldScale;
	void initModes();
	map<string, cameraEffect> _effects;
};

