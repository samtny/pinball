#include <stdio.h>

class PinballHost {
	public:
		PinballHost(void);
		~PinballHost(void);
		void init();
		void start(const char *gameName);
	private:
		void *_engine;
};
