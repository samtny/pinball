
#pragma once

#include <map>

#include "chipmunk/chipmunk.h"

#include "Parts/Flipper.h"

#define MAX_BALL_COUNT 10

class PinballBridgeInterface;
class IPhysicsDelegate;
class Game;

class Playfield;
struct LayoutItem;

#if CP_USE_CGPOINTS
typedef CGPoint cpVect;
#else
struct cpVect;
#endif

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

    Playfield *getPlayfield();
    
	IPhysicsDelegate *getDelegate();
	
	cpSpace *getSpace();
	
	float getScale();

	void updatePhysics();
	
	void resetBallsToInitialPosition();

	void setPaused(bool paused);

	bool getPaused();

	void createObjects(void);
	void createObject(LayoutItem *iprops);
	void destroyObject(LayoutItem *iprops);
	
	void activateMech(const char *mechName);
	void deactivateMech(const char *mechName);

	void nudge(cpVect dir);

	cpBody *getBoxBody();
    
    LayoutItem *getLayoutItem(const char *itemName);

protected:
	void loadConfig();
    void loadForces();
	void createBox(LayoutItem *iprops);
    void createBall(LayoutItem *iprops);
	void createDropTarget(LayoutItem *iprops);
	void createSwitch(LayoutItem *iprops);
	void createSegment(LayoutItem *iprops);
	void createCircle(LayoutItem *iprops);
    void createKraken(LayoutItem *iprops);
	void flip(LayoutItem *flipper);
	void unflip(LayoutItem *flipper);
private:
	PinballBridgeInterface *_bridgeInterface;
	IPhysicsDelegate *_delegate;
	Playfield *_playfield;
	cpSpace *_space;
	cpBody *_boxBody;
	bool _paused;
    
    std::map<std::string, Flipper(*)> flippers;
};

