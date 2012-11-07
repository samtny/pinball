#include "PinballHost.h"

#include "Physics.h"
#include "Renderer.h"

#include <cstddef>

PinballHostImpl::PinballHostImpl(void) {
	// noes?
}

PinballHostImpl::~PinballHostImpl(void) {
	// noes?
}

const char *PinballHostImpl::getPathForScriptFileName(void * scriptFileName) {
	// in c++, yes it is;
	return (const char *)scriptFileName;
}

PinballHostEnvironment *PinballHostImpl::getPinballHostEnvironment() {
	// TODO: something
	return NULL;
}

void PinballHostImpl::playSound(void *soundName) {
	// TODO: something
}

PinballHost::PinballHost(void)
{
}


PinballHost::~PinballHost(void)
{
}

void PinballHost::start(const char *gameName) {

	// TODO: init win32 stuff...

	Physics *physics = new Physics();
	Renderer *renderer = new Renderer();

	finished = false;
	paused = false;
	while (!finished) {
		if (!paused) {
			physics->updatePhysics();
		}
		renderer->draw();
	}

}