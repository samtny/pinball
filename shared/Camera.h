
using namespace std;
#include <string>
#include <vector>
#include <map>

class PinballBridgeInterface;
struct HostProperties;
class Playfield;
struct Coord2;

typedef enum CameraType {
	CAMERA_TYPE_FOLLOW_BALL,
	CAMERA_TYPE_FIXED
} CameraType;

typedef struct CameraEffect {
	string n;	
	float d;
	float aStart;
	float aEnd;
	double aCurrent;
	double startTime;
} CameraEffect;

struct CameraMode;

class Camera {
public:
	Camera();
	~Camera();
	void setBridgeInterface(PinballBridgeInterface *bridgeInterface);
	void setPlayfield(Playfield *playfield);
	void init();
	void setZoomLevel(float zoomLevel);
	float getZoomLevel();
	void setMode(const char *modeName);
	
	Coord2 transform(Coord2 coord);

	void applyTransform(void);

	void doEffect(const char *effectName);

	void applyEffectsTransforms(void);

protected:
	void loadConfig();
	void loadCamera();
	void loadEffects();
	void initModes();
private:
	PinballBridgeInterface *_bridgeInterface;
	HostProperties *_displayProperties;
	Playfield *_playfield;
	float _scale;
	float _minZoomLevel;
	float _maxZoomLevel;
	map<string, CameraEffect> _effects;
	vector<CameraEffect> _activeEffects;
	CameraType _type;
	map<string, CameraMode> _cameraModes;
	CameraMode *_activeCameraMode;
};


