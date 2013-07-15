#include <stdio.h>

class GlutEngine;

class PinballHost {
	public:
		PinballHost(void);
		~PinballHost(void);
		void init(const char *gameName);
		void start();
	private:
		GlutEngine *_engine;
};
