
#pragma once

// TODO: move typedefs out to new header and move folowing to implementation file
#include "PinballBridgeInterface.h"

class Physics;
class Camera;
struct layoutItem;
struct cpBody;

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
	void drawPlayfield(void);
	void drawObject(cpBody *body, void *data);
	void drawBall(layoutItem *item);
	void drawFonts(void);
	void setCameraMode(const char *modeName);
	void setZoomLevel(float zoomLevel);
	float getZoomLevel();
private:
	PinballBridgeInterface *_bridgeInterface;
	Physics *_physics;
	Camera *_camera;
	glfont::GLFont *_glfont;
	HostProperties *_displayProperties;
	float _scale;
};

