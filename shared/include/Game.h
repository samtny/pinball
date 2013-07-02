
#include "PhysicsDelegate.h"
#include "TimerDelegate.h"

#include <string>
#include <vector>

class PinballBridgeInterface;
class Physics;
class Renderer;
struct lua_State;

class Game : public IPhysicsDelegate, ITimerDelegate {
public:
	Game(void);
	~Game(void);
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void setPhysics(Physics *physics);
	void setRenderer(Renderer *renderer);
	void init();
	void switchClosed(const char *switchName);
	void switchOpened(const char *switchName);
	void closeSwitch(int switchIndex);
	void resetBallPosition();
	void setCameraMode(const char *modeName);
	void setZoomLevel(float zoomLevel);
	float getZoomLevel();
	void addLuaTimer(float duration, std::string funcName, int arg);
	void timerCallback(int timerId);
	void updateOverlayText(const char *key, const char *val);
	void doCameraEffect(const char *effectName);
	void activateMech(const char *mechName);
	void deactivateMech(const char *mechName);
    void playSound(const char *sound, const float loopInterval);
	void setPaused(bool paused);
	bool getPaused();
	const char *getGameName();
protected:
	void loadRules();
private:
	PinballBridgeInterface *_bridgeInterface;
	Physics *_physics;
	Renderer *_renderer;
	lua_State *_rules;
/*
	int _zoomLevel;
	int _maxZoomLevel;
	int _min_ZoomLevel;
*/
	bool _paused;
};

