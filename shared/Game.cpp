
#include "Game.h"

#include "PinballBridgeInterface.h"

#include "Physics.h"

#include "Renderer.h"

extern "C" {
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}

Game::Game(void) {

}

Game::~Game(void) {
	lua_close(_rules);
}

void Game::setBridgeInterface(PinballBridgeInterface *bridgeInterface) {
	_bridgeInterface = bridgeInterface;
}

void Game::setPhysics(Physics *physics) {
	_physics = physics;
	_physics->setDelegate(this);
}

void Game::setRenderer(Renderer *renderer) {
	_renderer = renderer;
}

static Game *lua_currentInstance;

void Game::init(void) {
	this->loadRules();
	lua_currentInstance = this;
}

static int lua_resetBallPosition(lua_State *L) {

	lua_currentInstance->resetBallPosition();
	return 0;

}

static int lua_setCameraFollowsBall(lua_State *L) {

	lua_currentInstance->setCameraFollowsBall();
	return 0;

}

void Game::loadRules(void) {
	
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *rulesFilename = _bridgeInterface->getPathForScriptFileName((void *)"rules.lua");

	int error = luaL_dofile(L, rulesFilename);
	if (!error) {

        // TODO: any init here from lua vars...?
		lua_pushcfunction(L, lua_resetBallPosition);
		lua_setglobal(L, "resetBallPosition");

		lua_pushcfunction(L, lua_setCameraFollowsBall);
		lua_setglobal(L, "setCameraFollowsBall");

    } else {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  // pop err from lua stack
	}

	//lua_close(L);

	_rules = L;

}

void Game::switchClosed(const char *switchName) {
	lua_getglobal(_rules, "handleSwitchClosed");
	lua_pushstring(_rules, switchName);
	lua_call(_rules, 1, 0);
}

// TODO: this is redundant (legacy) method;
void Game::closeSwitch(int switchIndex) {

	// TODO: this is all set dynamically / lookup table probably here...
	if (switchIndex == 0) {
		lua_getglobal(_rules, "handleSwitchClosed");
		lua_pushstring(_rules, "startButton");
		lua_call(_rules, 1, 0);
	}

}

void Game::resetBallPosition() {
	// TODO: something...
	_physics->resetBallPosition(0);
}

void Game::setCameraFollowsBall() {

	fprintf(stderr, "%s\n", "setCameraFollowsBall");
	_renderer->setCameraFollowsBall();	

}

void Game::setZoomLevel(float zoomLevel) {
	_renderer->setZoomLevel(zoomLevel);
}

float Game::getZoomLevel() {
	return _renderer->getZoomLevel();
}

