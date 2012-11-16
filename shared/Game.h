
#include "PhysicsDelegate.h"

class PinballBridgeInterface;
class Physics;
class Renderer;
struct lua_State;

class Game : public IPhysicsDelegate {
public:
	Game(void);
	~Game(void);
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void setPhysics(Physics *physics);
	void setRenderer(Renderer *renderer);
	void init();
	void switchClosed(const char *switchName);
	void closeSwitch(int switchIndex);
	void resetBallPosition();
	void setCameraFollowsBall();
	void setZoomLevel(float zoomLevel);
	float getZoomLevel();
protected:
	void loadRules();
private:
	PinballBridgeInterface *_bridgeInterface;
	Physics *_physics;
	Renderer *_renderer;
	lua_State *_rules;
	int _zoomLevel;
	int _maxZoomLevel;
	int _min_ZoomLevel;
};

