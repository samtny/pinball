#pragma once

class GlutEngine;

class PinballHost
{
public:
	PinballHost(void);
	~PinballHost(void);
	void init();
	void start();
private:
	GlutEngine *_glutEngine;
};

