#include "PinballHost.h"

#include "PinballBridgeInterface.h"

#include "Playfield.h"

#include "Physics.h"

#include "Camera.h"

#include "Editor.h"

#include "Renderer.h"

#include "Game.h"

#include "GlutEngine.h"

#include <iostream>

PinballHost::PinballHost() {
}

PinballHost::~PinballHost() {
}

void PinballHost::init(const char *gameName) {

	std::cout << "starting " << gameName << std::endl;
	PinballBridgeInterface *bi = new PinballBridgeInterface();
	bi->setGameName(gameName);
	
	Playfield *f = new Playfield();
	f->setBridgeInterface(bi);
	f->init();

	Physics *p = new Physics();
	p->setBridgeInterface(bi);
	p->setPlayfield(f);
	p->init();

	Camera *c = new Camera();
	c->setBridgeInterface(bi);
	c->setPlayfield(f);
	c->init();

	Editor *t = new Editor();
	t->setBridgeInterface(bi);
	t->setPlayfield(f);
	t->setCamera(c);
	t->setPhysics(p);
	t->init();

	GlutEngine *e = new GlutEngine();
	e->init();

	Renderer *r = new Renderer();
	r->setBridgeInterface(bi);
	r->setPlayfield(f);
	r->setPhysics(p);
	r->setCamera(c);
	r->setEditor(t);
	r->init();
	
	Game *g = new Game();
	g->setBridgeInterface(bi);
	g->setPhysics(p);
	g->setRenderer(r);
	g->init();
	
	// TODO: find factoring error/s
	e->setPhysics(p);
	e->setRenderer(r);
	e->setGame(g);
	e->setEditor(t);
	
	_engine = e;

}

void PinballHost::start(void) {
	_engine->start();
}
	
