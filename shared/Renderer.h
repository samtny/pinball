
#pragma once

// TODO: move typedefs out to new header and move folowing to implementation file
#include "PinballBridgeInterface.h"

class Physics;
namespace glfont
{
	class GLFont;	
}

class Renderer
{
public:
	Renderer(void);
	~Renderer(void);
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void setPhysics(Physics *physics);
	void init(void);
	void loadTextures(void);
	void loadFonts(void);
	void draw(void);
private:
	PinballBridgeInterface *_bridgeInterface;
	Physics *_physics;
	glfont::GLFont *_glfont;
	DisplayProperties *_displayProperties;
};

