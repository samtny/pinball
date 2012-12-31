#include "PinballHost.h"

#include "PinballBridgeInterface.h"

#include "Playfield.h"

#include "Physics.h"

#include "Renderer.h"

#include "Camera.h"

#include "Game.h"

#include "Editor.h"

#include "GlutEngine.h"

PinballHost::PinballHost() {
}

PinballHost::~PinballHost() {
}

void PinballHost::init() {

	PinballBridgeInterface *bi = new PinballBridgeInterface();
	bi->init();
	bi->setGameName("Pinferno");

	Playfield *f = new Playfield();
	f->setBridgeInterface(bi);
	f->init();

	Physics *p = new Physics();
	p->setBridgeInterface(bi);
	p->init();

	Camera *c = new Camera();
	c->setBridgeInterface(bi);
	c->setDisplayProperties(bi->getHostProperties());
	c->setPhysics(p);
	c->init();

	Editor *t = new Editor();
	t->setBridgeInterface(bi);
	t->setCamera(c);
	t->setPhysics(p);
	t->init();

	GlutEngine *e = new GlutEngine();
	e->init();

	Renderer *r = new Renderer();
	r->setBridgeInterface(bi);
	r->setPhysics(p);
	r->setCamera(c);
	r->setEditor(t);
	r->init();
	
	Game *g = new Game();
	g->setBridgeInterface(bi);
	g->setPhysics(p);
	g->setRenderer(r);
	g->init();
	
	
	e->setPhysics(p);
	e->setRenderer(r);
	e->setGame(g);
	e->setEditor(t);
	
	_glutEngine = e;

	// TODO: find factoring error;
	//r->init();
	
}

void PinballHost::start() {

	_glutEngine->start(); // for now...

}

