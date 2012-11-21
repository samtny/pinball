
typedef enum CameraType {
	CAMERA_TYPE_FOLLOW_BALL
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
	float minY;
	float maxY;
	float marginY;
	float minZoomLevel;
	float maxZoomLevel;
	void setDisplayProperties(HostProperties *displayProperties);
	void setRenderer(Renderer *renderer);
	void setPhysics(Physics *physics);
	void setZoomLevel(float zoomLevel);
	float getZoomLevel();
	void setWorldScale(float worldScale);
	void setModeFollowBall();
	void applyTransform(void);
protected:
	void loadConfig();
private:
	PinballBridgeInterface *_bridgeInterface;
	HostProperties *_displayProperties;
	Renderer *_renderer;
	Physics *_physics;
	float _zoomLevel;
	float _worldScale;
};

