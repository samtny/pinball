
class Physics;
class Renderer;
class Game;
class Editor;

class GlutEngine {
public:
	GlutEngine(void);
	~GlutEngine(void);
	void setPhysics(Physics *physics);
	void setRenderer(Renderer *renderer);
	void setGame(Game *game);
	void setEditor(Editor *editor);
	void init();
	void start();
	void timerFunc(int value);
	void displayFunc();
	void keyboardCallback(unsigned char key);
	void keyboardUpCallback(unsigned char key);
	void mouseCallback(int button, int state, int x, int y);
	void motionCallback(int x, int y);
	void menuCallback(int value);
	void menuCallbackInsert(int value);
private:
	Physics *_physics;
	Renderer *_renderer;
	Game *_game;
	Editor *_editor;
};

#define SLEEP_TICKS 16




