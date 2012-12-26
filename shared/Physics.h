
#pragma once

#define MAX_BALL_COUNT 10

class PinballBridgeInterface;
class IPhysicsDelegate;
class Game;

struct cpSpace;
struct cpBody;
struct cpArbiter;

struct materialProperties;
struct objectProperties;
struct layoutItem;

using namespace std;
#include <string>
#include <map>

class Physics
{
public:
	
	Physics(void);
	~Physics(void);
	
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void setDelegate(IPhysicsDelegate *delegate);
	void init();

	IPhysicsDelegate *getDelegate();
	
	cpSpace *getSpace();
	
	float getScale();

	map<string, layoutItem> *getLayoutItems();
	
	void updatePhysics();
	
	void resetBallsToInitialPosition();
	
	int ballPreSolve(cpArbiter *arb, cpSpace *space, void *unused);

	void setPaused(bool paused);

	bool getPaused();

	// TODO: refactor veriously...
	void applyScale(layoutItem *iprops);
	void createObject(layoutItem *iprops);
	void destroyObject(layoutItem *iprops);
	void destroyBody(cpBody *body);

	void addLayoutItem(layoutItem item);

	void activateMech(const char *mechName);
	void deactivateMech(const char *mechName);

protected:
	void loadConfig();
	void loadMaterials();
	void loadObjects();
	void loadLayout();
    void loadForces();
	void initCollisionHandlers();
	cpBody *createBox(layoutItem *iprops);
    cpBody *createBall(layoutItem *iprops);
	cpBody *createFlipper(layoutItem *iprops);
	cpBody *createTarget(layoutItem *iprops);
	cpBody *createPopbumper(layoutItem *iprops);
	cpBody *createSlingshot(layoutItem *iprops);
	void createSwitch(layoutItem *iprops);
	void createSegment(layoutItem *iprops);
	void createCircle(layoutItem *iprops);
	void flip(layoutItem *flipper);
	void unflip(layoutItem *flipper);
private:
	PinballBridgeInterface *_bridgeInterface;
	IPhysicsDelegate *_delegate;
	cpSpace *_space;
	map<string, materialProperties> _materials;
	map<string, objectProperties> _objects;
	map<string, layoutItem> _layoutItems;
	bool _paused;
};

