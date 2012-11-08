#pragma once

#include <string>
#include <map>
using namespace std;

struct layoutItemProperties;
struct objectProperties;
struct materialProperties;

struct cpBody;

class PinballNativeImpl;

class Physics
{
public:
	Physics(void);
	~Physics(void);
	void init(PinballNativeImpl *pinballNative);
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
	PinballNativeImpl *_pinballNativeImpl;
	cpBody *_box;
};

