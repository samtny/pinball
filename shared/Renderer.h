
#pragma once

// TODO: move typedefs out to new header and move folowing to implementation file
#include "PinballBridgeInterface.h"
#include "Parts.h"

#include <string>
#include <map>

class Physics;
class Camera;
class Editor;
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
	void setCamera(Camera *camera);
	void setEditor(Editor *editor);
	void init(void);
	void loadTextures(void);
	void loadFonts(void);
	void loadOverlays(void);
	void draw(void);
	void drawPlayfield(void);
	void drawObject(cpBody *body, void *data);
	void drawAnchors(layoutItem *item);
	void drawBall(layoutItem *item);
	void drawBox(layoutItem *item);
	void drawOverlays(void);
	void setCameraMode(const char *modeName);
	void setZoomLevel(float zoomLevel);
	void setOverlayText(const char *overlayName, const char *text);
	void doCameraEffect(const char *effectName);
	float getZoomLevel();
private:
	PinballBridgeInterface *_bridgeInterface;
	Physics *_physics;
	Camera *_camera;
	Editor *_editor;
	glfont::GLFont *_glfont;
	HostProperties *_displayProperties;
	map<string, textureProperties> _textures;
	map<string, overlayProperties> _overlays;
	map<string, layoutItem> *_layoutItems;
	float _scale;
};

