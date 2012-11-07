#pragma once

using namespace std;
#include <string>
#include <map>

struct layoutItemProperties;
struct objectProperties;
struct materialProperties;

struct cpBody;

class PinballHostImpl;

class Physics
{
public:
	Physics(void);
	~Physics(void);
	PinballHostImpl *_pinballHostImpl;
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
	cpBody *_box;
};

