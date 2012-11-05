
#include "Game.h"
#include "Playfield.h"
#include "Physics.h"
#include "Renderer.h"

void Game::start() {

	Playfield *p = new Playfield();
	p->init("PorkyPinball.lua");

	Physics *e = new Physics();

	Renderer *r = new Renderer();
	r->physicsEngine = e;
	r->init(p, e);

	while (!finished) {
		if (!paused) {
			e->updatePhysics();
		}
		r->draw();
	}

}