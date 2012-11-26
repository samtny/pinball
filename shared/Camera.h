
typedef enum CameraType {
	CAMERA_TYPE_FOLLOW_BALL,
	CAMERA_TYPE_FIXED
} CameraType;

// TODO: move out;
#include "PinballBridgeInterface.h"

class Renderer;
class Physics;
struct layoutItem;

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
protected:
	void loadConfig();
	void loadCamera();
private:
	PinballBridgeInterface *_bridgeInterface;
	HostProperties *_displayProperties;
	Physics *_physics;
	float _worldScale;
	void initModes();
};

