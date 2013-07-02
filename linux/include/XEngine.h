class Physics;
class Renderer;
class Game;
class Editor;

class XEngine {
	public:
		XEngine(void);
		~XEngine(void);
		void init();
		void setPhysics(Physics *p);
		void setRenderer(Renderer *r);
		void setGame(Game *g);
		void setEditor(Editor *e);
	private:
		Physics *_physics;
		Renderer *_renderer;
		Game *_game;
		Editor *_editor;
};
