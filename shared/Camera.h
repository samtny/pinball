
enum {
	CAMERA_MODE_FOLLOW_BALL
};

class Renderer;
class Physics;
struct layoutItem;

class Camera {
public:
	Camera();
	~Camera();
	int mode;
	float minY;
	float maxY;
	float marginY;
	float minZoomLevel;
	float maxZoomLevel;
	void setRenderer(Renderer *renderer);
	void setPhysics(Physics *physics);
	void setZoomLevel(float zoomLevel);
	float getZoomLevel();
	void setWorldScale(float worldScale);
	void setModeFollowBall();
	void applyTransform(void);
private:
	Renderer *_renderer;
	Physics *_physics;
	float _zoomLevel;
	float _worldScale;
};

