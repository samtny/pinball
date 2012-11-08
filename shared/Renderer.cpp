#include "Renderer.h"

#include "PinballNativeInterface.h"

Renderer::Renderer(void)
{

}

Renderer::~Renderer(void)
{

}

void Renderer::init(PinballNativeImpl *pinballNative) {
	this->_pinballNative = pinballNative;
}

void Renderer::draw() {

}

