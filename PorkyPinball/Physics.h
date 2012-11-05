#pragma once

struct layoutItemProperties;
struct objectProperties;
struct materialProperties;

class Physics
{
public:
	Physics(void);
	~Physics(void);
	void updatePhysics();
protected:
	void loadMaterials();
	void loadObjects();
	void loadLayout();
	void createObject(string name, layoutItemProperties iprops, objectProperties oprops, materialProperties mprops);
};

