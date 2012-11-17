
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
	float margin;
	float minZoomLevel;
	float maxZoomLevel;
	float zoomLevel;
	void setRenderer(Renderer *renderer);
	void setPhysics(Physics *physics);
	void setModeFollowBall(layoutItem *ball, layoutItem *box);
	void applyTransform(void);
private:
	Renderer *_renderer;
	Physics *_physics;
};

