
#pragma once

#define MAX_BALL_COUNT 10

class PinballBridgeInterface;
class IPhysicsDelegate;
class Game;

struct cpBody;
struct cpSpace;

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
	void init(PinballBridgeInterface *bridgeInterface);
	void setDelegate(IPhysicsDelegate *delegate);
	IPhysicsDelegate *getDelegate();
	cpSpace *getSpace();
	void getLayoutItems(const layoutItem *items, int size);
	const layoutItem *getBox();
	const layoutItem *getBalls(layoutItem *balls, int size);
	void updatePhysics();
	void resetBallPosition(layoutItem *ball);
protected:
	void loadConfig();
	void loadMaterials();
	void loadObjects();
	void loadLayout();
    void loadForces();
	void initCollisionHandlers();
	void applyScale(layoutItem *iprops);
	void createObject(layoutItem *iprops);
	cpBody *createBox(layoutItem *iprops);
    cpBody *createBall(layoutItem *iprops);
	cpBody *createFlipper(layoutItem *iprops);
	void createSwitch(layoutItem *iprops);
	void createSegment(layoutItem *iprops);
private:
	PinballBridgeInterface *_bridgeInterface;
	IPhysicsDelegate *_delegate;
	cpSpace *_space;
	layoutItem *_box;
	layoutItem *_balls[MAX_BALL_COUNT];
	map<string, materialProperties> *_materials;
	map<string, objectProperties> *_objects;
	map<string, layoutItem> *_layoutItems;
	int _ballCount;
};

