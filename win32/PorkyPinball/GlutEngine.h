
class Physics;
class Renderer;

class GlutEngine {
public:
	GlutEngine(void);
	~GlutEngine(void);
	void setPhysics(Physics *physics);
	void setRenderer(Renderer *renderer);
	void init();
	void start();
	void timerFunc(int value);
	void displayFunc();
private:
	Physics *_physics;
	Renderer *_renderer;
};

#define SLEEP_TICKS 16




