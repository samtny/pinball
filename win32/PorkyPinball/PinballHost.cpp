#include "PinballHost.h"

#include "PinballBridgeInterface.h"

#include "Physics.h"

#include "Renderer.h"

#include "Camera.h"

#include "Game.h"

#include "GlutEngine.h"

PinballHost::PinballHost() {
}

PinballHost::~PinballHost() {
}

void PinballHost::init() {

	PinballBridgeInterface *bi = new PinballBridgeInterface();
	bi->init();
	bi->setGameName("Flipperfall");

	GlutEngine *e = new GlutEngine();
	e->init();
	
	Physics *p = new Physics();
	p->setBridgeInterface(bi);
	p->init();

	Renderer *r = new Renderer();
	r->setBridgeInterface(bi);
	r->setPhysics(p);
	r->init();
	
	Game *g = new Game();
	g->setBridgeInterface(bi);
	g->setPhysics(p);
	g->setRenderer(r);
	g->init();
	
	e->setPhysics(p);
	e->setRenderer(r);
	e->setGame(g);
	_glutEngine = e;

	// TODO: find factoring error;
	//r->init();
	
}

void PinballHost::start() {

	_glutEngine->start(); // for now...

}

