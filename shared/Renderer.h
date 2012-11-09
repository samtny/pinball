
#pragma once

// TODO: move typedefs out to new header and move folowing to implementation file
#include "PinballBridgeInterface.h"

class Physics;

class Renderer
{
public:
	Renderer(void);
	~Renderer(void);
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void setPhysics(Physics *physics);
	void init(void);
	void draw(void);
private:
	PinballBridgeInterface *_bridgeInterface;
	Physics *_physics;
	DisplayProperties *_displayProperties;
};

