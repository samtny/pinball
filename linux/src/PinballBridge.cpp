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



