
class Physics;
class Renderer;
class Game;

class GlutEngine {
public:
	GlutEngine(void);
	~GlutEngine(void);
	void setPhysics(Physics *physics);
	void setRenderer(Renderer *renderer);
	void setGame(Game *game);
	void init();
	void start();
	void timerFunc(int value);
	void displayFunc();
	void keyboardCallback(unsigned char key);
	void keyboardUpCallback(unsigned char key);
private:
	Physics *_physics;
	Renderer *_renderer;
	Game *_game;
};

#define SLEEP_TICKS 16




