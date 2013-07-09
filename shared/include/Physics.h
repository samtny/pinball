
#pragma once

#define MAX_BALL_COUNT 10

class PinballBridgeInterface;
class IPhysicsDelegate;
class Game;

class Playfield;
struct LayoutItem;

struct cpVect;
struct cpSpace;
struct cpBody;
struct cpArbiter;

class Physics
{
public:
	
	Physics(void);
	~Physics(void);
	
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void setDelegate(IPhysicsDelegate *delegate);
	void setPlayfield(Playfield *playfield);
	void init();

	IPhysicsDelegate *getDelegate();
	
	cpSpace *getSpace();
	
	float getScale();

	void updatePhysics();
	
	void resetBallsToInitialPosition();
	
	int ballPreSolve(cpArbiter *arb, cpSpace *space, void *unused);

	void setPaused(bool paused);

	bool getPaused();

	void createObjects(void);
	void createObject(LayoutItem *iprops);
	void destroyObject(LayoutItem *iprops);
	
	void activateMech(const char *mechName);
	void deactivateMech(const char *mechName);

	void nudge(cpVect dir);

	cpBody *getBoxBody();

protected:
	void loadConfig();
    void loadForces();
	void initCollisionHandlers();
	void createBox(LayoutItem *iprops);
    void createBall(LayoutItem *iprops);
	void createFlipper(LayoutItem *iprops);
	void createTarget(LayoutItem *iprops);
	void createDropTarget(LayoutItem *iprops);
	void createPopbumper(LayoutItem *iprops);
	void createSlingshot(LayoutItem *iprops);
	void createSwitch(LayoutItem *iprops);
	void createSegment(LayoutItem *iprops);
	void createCircle(LayoutItem *iprops);
	void flip(LayoutItem *flipper);
	void unflip(LayoutItem *flipper);
private:
	PinballBridgeInterface *_bridgeInterface;
	IPhysicsDelegate *_delegate;
	Playfield *_playfield;
	cpSpace *_space;
	cpBody *_boxBody;
	bool _paused;
};

