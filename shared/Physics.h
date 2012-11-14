#pragma once

#include <string>
#include <map>
using namespace std;

struct cpSpace;
struct cpBody;

// TODO: move typedefs up and/or out...
#include "chipmunk/chipmunk.h"
typedef struct materialProperties {
	string n;
	float e;
	float f;
	float d;
} materialProperties;
typedef map<string, materialProperties>::iterator it_materialProperties;

typedef struct objectTextureProperties {
	string n;
	int x;
	int y;
	int w;
	int h;
	float s;
	float a;
} objectTextureProperties;

typedef struct objectProperties {
	string n;
	string s;
	float r1;
	float r2;
	materialProperties m;
	objectTextureProperties t;
} objectProperties;
typedef map<string, objectProperties>::iterator it_objectProps;

typedef struct layoutItemProperties {
	string n;
	objectProperties o;
	cpVect v[200];
	int count;
} layoutItemProperties;
typedef map<string, layoutItemProperties>::iterator it_layoutItems;

class PinballBridgeInterface;

class Physics
{
public:
	Physics(void);
	~Physics(void);
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void init();
	float getBoxWidth();
	cpSpace *getSpace();
	void updatePhysics();
	void resetBallPosition(int ballIndex);
	map<string, materialProperties> materials;
	map<string, objectProperties> objects;
	map<string, layoutItemProperties> layoutItems;
	cpBody *_box;
	cpBody *_balls[10];
protected:
	void loadConfig();
	void loadMaterials();
	void loadObjects();
	void loadLayout();
    void loadForces();
	void applyScale(layoutItemProperties *iprops);
	void createObject(layoutItemProperties *iprops);
	void createBox(layoutItemProperties *iprops);
    void createBall(layoutItemProperties *iprops);
	void createFlipper(layoutItemProperties *iprops);
	void createSegment(layoutItemProperties *iprops);
private:
	PinballBridgeInterface *_bridgeInterface;
	float _boxWidth;
};

