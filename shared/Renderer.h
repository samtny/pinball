
#ifndef __PINBALL_RENDERER__
#define __PINBALL_RENDERER__

#include <string>
#include <map>

class PinballBridgeInterface;
struct HostProperties;
class Physics;
class Playfield;
class Camera;
class Editor;
struct LayoutItem;
struct Texture;
struct Overlay;

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
	void setPlayfield(Playfield *playfield);
	void setPhysics(Physics *physics);
	void setCamera(Camera *camera);
	void setEditor(Editor *editor);
	void init(void);
	void loadFonts(void);
	void loadOverlays(void);
	void draw(void);
	void drawPlayfield(void);
	void drawObject(LayoutItem *item);
	void drawAnchors(LayoutItem *item);
	void drawBall(LayoutItem *item);
	void drawBox(LayoutItem *item);
	void drawOverlays(void);
	void setCameraMode(const char *modeName);
	void setZoomLevel(float zoomLevel);
	void setOverlayText(const char *overlayName, const char *text);
	void doCameraEffect(const char *effectName);
	float getZoomLevel();
private:
	PinballBridgeInterface *_bridgeInterface;
	Playfield *_playfield;
	Physics *_physics;
	Camera *_camera;
	Editor *_editor;
	glfont::GLFont *_glfont;
	HostProperties *_displayProperties;
	map<string, Overlay> *_overlays;
	float _scale;
};

#endif

