
#include "Game.h"

#include "PinballNativeInterface.h"

#include "Physics.h"

#include "Renderer.h"

Game::Game(void) {

}

Game::~Game(void) {

}

void Game::start(void) {
	
	_pinballNative = new PinballNativeImpl();
	_pinballNative->init();

	Physics *physics = new Physics();
	physics->init(_pinballNative);

	Renderer *renderer = new Renderer();
	//renderer->init(_pinballNative);

	finished = false;
	paused = false;
	while (!finished) {
		if (!paused) {
			physics->updatePhysics();
		}
		renderer->draw();
	}

	delete _pinballNative;

}

