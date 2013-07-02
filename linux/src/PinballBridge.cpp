#include "PinballBridge.h"
#include <iostream>
#include <cstring>
#include <unistd.h>

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
	if (_basePath == NULL) {
		char cwd[1024];
		getcwd(cwd, sizeof(cwd));
		_basePath = (const char *)&cwd;
	}
	return _basePath;	
}

const char *PinballBridgeInterface::getBasePath() {
	return static_cast<PinballBridge *>(_this)->getBasePath();
}

const char *PinballBridge::getScriptPath(const char *_scriptName) {
	if (_scriptPath == NULL) {
		char path[1024];
		strcpy(path, _basePath);
		strcat(path, "/../shared/resource/");
		strcat(path, _gameName);
		strcat(path, "/");
		strcat(path, _scriptName);
		_scriptPath = (const char *)&path;
	}
	return _scriptPath;
}

const char *PinballBridgeInterface::getScriptPath(const char *scriptName) {
	return static_cast<PinballBridge *>(_this)->getScriptPath(scriptName);
}

const char *PinballBridge::getTexturePath(const char *_textureName) {
	if (_texturePath == NULL) {
		char path[1024];
		strcpy(path, _basePath);
		strcat(path, "/../shared/resource/");
		strcat(path, _gameName);
		strcat(path, "/");
		strcat(path, _textureName);
		_texturePath = (const char *)&path;
	}
	return _texturePath;
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
	return NULL;
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



