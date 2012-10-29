
#include "PPGame.h"
#include "PPPlayfield.h"
#include "PPPhysicsEngine.h"
#include "PPRenderer.h"

void PPGame::start() {

	PPPlayfield *p = new PPPlayfield();
	p->init("PorkyPinball");

	PPPhysicsEngine *e = new PPPhysicsEngine();
	e->init(p);

	PPRenderer *r = new PPRenderer();
	r->physicsEngine = e;
	r->init(p, e);

	while (!finished) {
		if (!paused) {
			e->updatePhysics();
		}
		r->draw();
	}

}