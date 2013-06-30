class XEngine {
	public:
		XEngine(void);
		~XEngine(void);
		void init();
		void setPhysics(Physics *p);
		void setRenderer(Renderer *r);
		void setGame(Game *g);
		void setEditor(Editor *e);
};
