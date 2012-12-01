#include "PinballBridge.h"

#include <GL/glut.h>

#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")
#pragma comment(lib, "ILUT.lib")

#include <IL/il.h>

#include <sstream>
#include <string>
using namespace std;

PinballBridgeInterface::PinballBridgeInterface(void) : _this(nullptr) {
	
}

PinballBridgeInterface::~PinballBridgeInterface(void) {
	delete _this;
}

void PinballBridgeInterface::init() {
	_this = new PinballBridge();
	PinballBridge *b = (PinballBridge *)_this;
	b->init();
}

void PinballBridgeInterface::setGameName(const char *gameName) {
	PinballBridge *instance = (PinballBridge *)_this;
	instance ->setGameName(gameName);
}

const char * PinballBridgeInterface::getPathForScriptFileName(void * scriptFileName) {
	PinballBridge *nativeInstance = (PinballBridge *)_this;
	return nativeInstance->getPathForScriptFileName(scriptFileName);
}

const char *PinballBridgeInterface::getPathForTextureFileName(void *textureFilename) {
	PinballBridge *nativeInstance = (PinballBridge *)_this;
	return nativeInstance->getPathForTextureFilename(textureFilename);
}

Texture *PinballBridgeInterface::createRGBATexture(void *textureFilename) {
	PinballBridge *nativeInstance = (PinballBridge *)_this;
	return nativeInstance->createRGBATexture(textureFilename);
}

HostProperties * PinballBridgeInterface::getHostProperties() {
	PinballBridge *nativeInstance = (PinballBridge *)_this;
	return nativeInstance->getHostProperties();
}

void PinballBridgeInterface::addTimer(float duration, int timerId) {
	PinballBridge *instance = (PinballBridge *)_this;
	instance->addTimer(duration, timerId);
}

void PinballBridgeInterface::setTimerDelegate(ITimerDelegate *timerDelegate) {
	PinballBridge *instance = (PinballBridge *)_this;
	instance->setTimerDelegate(timerDelegate);
}

PinballBridge::PinballBridge(void)
{
}


PinballBridge::~PinballBridge(void)
{
}

void PinballBridge::init() {

}

void PinballBridge::setGameName(const char *gameName) {
	_gameName = gameName;
}

void PinballBridgeInterface::playSound(void * soundName) {
	// TODO: something
}

const char *PinballBridge::getPathForScriptFileName(void *scriptFileName) {
	
	const char *p = "..\\..\\shared\\";
	const char *g = _gameName;
	const char *s = "\\";
	const char *f = (const char *)scriptFileName;
	
	string path = "..\\..\\shared\\" + (string)_gameName + "\\" + (string)(const char *)scriptFileName;

	char *concat = new char[strlen(p) + strlen(g) + strlen(s) + strlen(f) + 1];
	strcpy(concat, p);
	strcat(concat, g);
	strcat(concat, s);
	strcat(concat, f);

	return concat;

}

const char *PinballBridge::getPathForTextureFilename(void *textureFilename) {
	
	const char *p = "..\\..\\shared\\";
	const char *g = _gameName;
	const char *s = "\\textures\\";
	const char *f = (const char *)textureFilename;
	
	char *concat = new char[strlen(p) + strlen(g) + strlen(s) + strlen(f) + 1];
	strcpy(concat, p);
	strcat(concat, g);
	strcat(concat, s);
	strcat(concat, f);

	return concat;

}

Texture *PinballBridge::createRGBATexture(void *textureFilename) {

	Texture *tex = new Texture();

	ilInit();
	ILuint imageName;
	ilGenImages(1, &imageName);
	ilBindImage(imageName);
	const char *filename = this->getPathForTextureFilename(textureFilename);
	ilLoadImage(filename);
	
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	
	tex->bpp = ilGetInteger(IL_IMAGE_BPP);
	tex->width = ilGetInteger(IL_IMAGE_WIDTH);
	tex->height = ilGetInteger(IL_IMAGE_HEIGHT);
	tex->data = ilGetData();

	return tex;

}

HostProperties *PinballBridge::getHostProperties() {

	HostProperties *props = new HostProperties();

	// TODO: vary by glut props;
	props->viewportX = 0;
	props->viewportY = 0;
	props->viewportHeight = 800;
	props->viewportWidth = 800;
	props->fontScale = 1;
	props->overlayScale = 1;
	return props;

}

static PinballBridge *currentInstance;

static void timerCallback(int timerId) {
	fprintf(stderr, "timer: %d\n", timerId);
	currentInstance->getTimerDelegate()->timerCallback(timerId);
}

ITimerDelegate *PinballBridge::getTimerDelegate() {
	return _timerDelegate;
}

void PinballBridge::setTimerDelegate(ITimerDelegate *timerDelegate) {
	_timerDelegate = timerDelegate;
}

void PinballBridge::addTimer(float duration, int timerId) {
	currentInstance = this;
	glutTimerFunc(duration * 1000, timerCallback, timerId);
}