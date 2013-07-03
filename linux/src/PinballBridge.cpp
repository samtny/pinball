#include "PinballBridge.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <libgen.h>
#include <Magick++.h>

PinballBridge::PinballBridge(void) {
}

PinballBridgeInterface::PinballBridgeInterface(void) : _this(nullptr) {
	_this = new PinballBridge();
}

void PinballBridge::setGameName(const char *gameName) {
	_gameName = gameName;
}

void PinballBridgeInterface::setGameName(const char *gameName) {
	static_cast<PinballBridge *>(_this)->setGameName(gameName);
}

const char *PinballBridge::getBasePath() {
	char *cwd = getcwd(0, 0);
	char *dir = dirname(cwd);
	return dir;	
}

const char *PinballBridgeInterface::getBasePath() {
	return static_cast<PinballBridge *>(_this)->getBasePath();
}

const char *PinballBridge::getScriptPath(const char *scriptName) {
	const char *p = "./../shared/resource/";
	const char *g = _gameName;
	const char *s = "/";
	const char *f = (const char *)scriptName;
	
	std::string path = "./../shared/" + (std::string)_gameName + "/" + (std::string)(const char *)scriptName;

	char *concat = new char[strlen(p) + strlen(g) + strlen(s) + strlen(f) + 1];
	strcpy(concat, p);
	strcat(concat, g);
	strcat(concat, s);
	strcat(concat, f);

	return concat;
}

const char *PinballBridgeInterface::getScriptPath(const char *scriptName) {
	return static_cast<PinballBridge *>(_this)->getScriptPath(scriptName);
}

const char *PinballBridge::getTexturePath(const char *textureName) {
	long size = 1024;
	char *buf = (char *)malloc((size_t)size);
	char *cwd = getcwd(buf, size);
	const char *d = dirname(cwd);
	const char *r = "/shared/resource/";
	const char *g = _gameName;
	const char *s = "/textures/";
	const char *f = (const char *)textureName;
	
	char *concat = new char[strlen(d) + strlen(r) + strlen(g) + strlen(s) + strlen(f) + 1];
	strcpy(concat, d);
	strcat(concat, r);
	strcat(concat, g);
	strcat(concat, s);
	strcat(concat, f);
	
	return concat;
}

const char *PinballBridgeInterface::getTexturePath(const char *textureName) {
	return static_cast<PinballBridge *>(_this)->getTexturePath(textureName);
}

const HostProperties *PinballBridge::getHostProperties() {
	if (_hostProperties == NULL) {
		HostProperties *props = new HostProperties();

		// TODO: vary by glut props;
		props->viewportX = 0;
		props->viewportY = 0;
		props->viewportHeight = 800;
		props->viewportWidth = 800;
		props->fontScale = 1;
		props->overlayScale = 1;

		_hostProperties = props;
	}
	return _hostProperties;	
}

const HostProperties *PinballBridgeInterface::getHostProperties() {
	return static_cast<PinballBridge *>(_this)->getHostProperties();
}

GLTexture *PinballBridge::createRGBATexture(const char *textureName) {
	GLTexture *tex = new GLTexture();
	
	const char *filename = this->getTexturePath(textureName);

	Magick::Image *i = new Magick::Image;
	

	i->read(filename);

	size_t width = i->columns();
	size_t height = i->rows();
	std::cout << "width: " << width << "\n";
	std::cout << "height: " << height << "\n";
	char *data = (char *)malloc(width * height * 4);
	i->write(0, 0, width, height, "RGBA", Magick::CharPixel, data);

	tex->width = width;
	tex->height = height;
	tex->data = data;
		
		
	return tex;
}

GLTexture *PinballBridgeInterface::createRGBATexture(void *textureName) {
	return static_cast<PinballBridge *>(_this)->createRGBATexture((const char *)textureName);
}

void PinballBridge::playSound(const char *soundName) {
	
}

void PinballBridgeInterface::playSound(const char *soundName) {
	static_cast<PinballBridge *>(_this)->playSound(soundName);
}

void PinballBridge::addTimer(float duration, int id, const ITimerDelegate *delegate) {
}

void PinballBridgeInterface::addTimer(float duration, int id, const ITimerDelegate *delegate) {
	static_cast<PinballBridge *>(_this)->addTimer(duration, id, delegate);
}

const char *PinballBridge::getGameName() {
	return _gameName;
}

const char *PinballBridgeInterface::getGameName() {
	return static_cast<PinballBridge *>(_this)->getGameName();
}



