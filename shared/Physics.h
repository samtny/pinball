#pragma once

#include <string>
#include <map>
using namespace std;

struct layoutItemProperties;
struct objectProperties;
struct materialProperties;

struct cpSpace;
struct cpBody;

class PinballBridgeInterface;

class Physics
{
public:
	Physics(void);
	~Physics(void);
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void init();
	cpSpace *getSpace();
	void updatePhysics();
protected:
	void loadMaterials();
	void loadObjects();
	void loadLayout();
	void createObject(string name, layoutItemProperties iprops, objectProperties oprops, materialProperties mprops);
	void createBox(string name, layoutItemProperties iprops, objectProperties oprops, materialProperties mprops);
	void createFlipper(string name, layoutItemProperties iprops, objectProperties oprops, materialProperties mprops);
	void createSegment(string name, layoutItemProperties iprops, objectProperties oprops, materialProperties mprops);
private:
	PinballBridgeInterface *_bridgeInterface;
	cpBody *_box;
};

