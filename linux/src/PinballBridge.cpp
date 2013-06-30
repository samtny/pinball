#include "PinballBridge.h"
#include <iostream>

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

