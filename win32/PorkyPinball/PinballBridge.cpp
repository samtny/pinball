#include "PinballBridge.h"

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

DisplayProperties * PinballBridgeInterface::getDisplayProperties() {
	PinballBridge *nativeInstance = (PinballBridge *)_this;
	return nativeInstance->getDisplayProperties();
}

PinballBridge::PinballBridge(void)
{
}


PinballBridge::~PinballBridge(void)
{
}

void PinballBridge::init() {

}

void PinballBridgeInterface::playSound(void * soundName) {
	// TODO: something
}

const char *PinballBridge::getPathForScriptFileName(void *scriptFileName) {
	
	const char *p = "..\\..\\shared\\PorkyPinball\\";
	const char *f = (const char *)scriptFileName;
	// TODO: this is a leak;
	char *concat = new char[strlen(p) + strlen(f) + 1];
	strcpy(concat, p);
	strcat(concat, f);
	return concat;

}

const char *PinballBridge::getPathForTextureFilename(void *textureFilename) {
	
	const char *p = "..\\..\\shared\\PorkyPinball\\textures\\";
	const char *f = (const char *)textureFilename;
	// TODO: this is a leak;
	char *concat = new char[strlen(p) + strlen(f) + 1];
	strcpy(concat, p);
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

DisplayProperties *PinballBridge::getDisplayProperties() {

	DisplayProperties *props = new DisplayProperties();

	// TODO: vary by glut props;
	props->viewportX = 0;
	props->viewportY = 0;
	props->viewportHeight = 600;
	props->viewportWidth = 800;
	return props;

}

