#include "PinballHost.h"

#include "PinballBridgeInterface.h"

#include "Physics.h"

#include "Renderer.h"

#include "Game.h"

#include "GlutEngine.h"

PinballHost::PinballHost() {
}

PinballHost::~PinballHost() {
}

void PinballHost::init() {

	PinballBridgeInterface *bi = new PinballBridgeInterface();
	bi->init();

	Physics *p = new Physics();
	p->setBridgeInterface(bi);
	p->init();

	Renderer *r = new Renderer();
	r->setBridgeInterface(bi);
	r->setPhysics(p);
	//r->init();
	
	Game *g = new Game();
	g->setBridgeInterface(bi);
	g->init();
	g->setPhysics(p);

	GlutEngine *e = new GlutEngine();
	e->setPhysics(p);
	e->setRenderer(r);
	e->setGame(g);
	e->init();
	_glutEngine = e;
	
}

void PinballHost::start() {

	_glutEngine->start(); // for now...

}

