#include "PinballNative.h"

#include "glut.h"

#include <sstream>
#include <string>
using namespace std;

PinballNativeImpl::PinballNativeImpl(void) : _this(nullptr) {
	
}

PinballNativeImpl::~PinballNativeImpl(void) {
	delete _this;
}

void PinballNativeImpl::init() {
	_this = new PinballNative();
	PinballNative *i = (PinballNative *)_this;
	i->init();
}

const char * PinballNativeImpl::getPathForScriptFileName(void * scriptFileName) {
	PinballNative *nativeInstance = (PinballNative *)_this;
	return nativeInstance->getPathForScriptFileName(scriptFileName);
}

DisplayProperties * PinballNativeImpl::getDisplayProperties() {
	PinballNative *nativeInstance = (PinballNative *)_this;
	return nativeInstance->getDisplayProperties();
}

PinballNative::PinballNative(void)
{
}


PinballNative::~PinballNative(void)
{
}

void PinballNative::init() {
	this->initOpenGl();
}

void PinballNative::initOpenGl() {
	
	int n = 0;
	char* v[1];
	v[0] = "";

	//glutInit(&n, v);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 50);
	glutCreateWindow("PorkyPinball");

	

}

void PinballNativeImpl::playSound(void * soundName) {
	// TODO: something
}

const char *PinballNative::getPathForScriptFileName(void *scriptFileName) {
	
	const char *p = "..\\..\\shared\\PorkyPinball\\";
	const char *f = (const char *)scriptFileName;
	// TODO: this is a leak;
	char *concat = new char[strlen(p) + strlen(f) + 1];
	strcpy(concat, p);
	strcat(concat, f);
	return concat;

}

DisplayProperties * PinballNative::getDisplayProperties() {
	DisplayProperties *props = new DisplayProperties();
	return props;
}
