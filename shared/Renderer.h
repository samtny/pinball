
#pragma once

// TODO: move typedefs out to new header and move folowing to implementation file
#include "PinballBridgeInterface.h"

class Physics;
namespace glfont
{
	class GLFont;	
}

struct Camera;
struct layoutItemProperties;

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
	void drawBall(layoutItemProperties layoutItem);
	void drawPlayfield(void);
	void drawFonts(void);
	void setCameraFollowsBall(void);
	void setZoomLevel(float zoomLevel);
	float getZoomLevel();
protected:
	void applyCameraTransform(void);
private:
	PinballBridgeInterface *_bridgeInterface;
	Physics *_physics;
	Camera *_camera;
	glfont::GLFont *_glfont;
	DisplayProperties *_displayProperties;
	GLfloat _scale;
};

