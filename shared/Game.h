
class PinballBridgeInterface;
class Physics;
class Renderer;
struct lua_State;

class Game {
public:
	Game(void);
	~Game(void);
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void setPhysics(Physics *physics);
	void setRenderer(Renderer *renderer);
	void init();
	void closeSwitch(int switchIndex);
	void resetBallPosition();
	void setCameraFollowsBall();
protected:
	void loadRules();
private:
	PinballBridgeInterface *_bridgeInterface;
	Physics *_physics;
	Renderer *_renderer;
	lua_State *_rules;
};

