
#include <string>
#include <vector>
#include <map>

class PinballBridgeInterface;
struct HostProperties;
class Playfield;
struct Coord2;

#include "Types.h"

typedef enum CameraType {
	CAMERA_TYPE_FOLLOW_BALL,
	CAMERA_TYPE_FIXED
} CameraType;

typedef struct CameraEffect {
	std::string n;	
	float d;
	float aStart;
	float aEnd;
	double aCurrent;
	double startTime;
} CameraEffect;

typedef struct CameraMode {
	std::string name;
	CameraType t;
	Coord2 c;
	float w;
	Coord2 b; // buffer / border
	float z;
} CameraMode;
typedef std::map<std::string, CameraMode>::iterator it_CameraMode;

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
	Coord2 scale(Coord2 coord);

	void applyTransform(void);

	void doEffect(const char *effectName);

	void applyEffectsTransforms(void);

	void setPan(const Coord2 pan);

	Coord2 getPan();

protected:
	void loadConfig();
	void loadCamera();
	void loadEffects();
	void initModes();
private:
	PinballBridgeInterface *_bridgeInterface;
	const HostProperties *_displayProperties;
	Playfield *_playfield;
	float _scale;
	float _minZoomLevel = 0.1f;
	float _maxZoomLevel = 3.0f;
	std::map<std::string, CameraEffect> _effects;
	std::vector<CameraEffect> _activeEffects;
	CameraType _type;
	std::map<std::string, CameraMode> _cameraModes;
	CameraMode _activeCameraMode;
	Coord2 _pan;
};


