
class PinballBridgeInterface;
class Physics;
struct lua_State;

class Game {
public:
	Game(void);
	~Game(void);
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void setPhysics(Physics *physics);
	void init();
	void closeSwitch(int switchIndex);
	void resetBallPosition();
protected:
	void loadRules();
private:
	PinballBridgeInterface *_bridgeInterface;
	Physics *_physics;
	lua_State *_rules;
};

