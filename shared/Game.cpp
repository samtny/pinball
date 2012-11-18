
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
	_bridgeInterface->setTimerDelegate(this);
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

struct luaTimer {
	int id;
	float duration;
	string funcName;
	int arg;
};

static int sNextTimerId = 0;
int nextTimerId() {
	return ++sNextTimerId;
}

static vector<luaTimer> timers;

static int lua_addTimer(lua_State *L) {

	int count = lua_gettop(L);

	float duration = (float)lua_tonumber(L, 1);
	string funcName = lua_tostring(L, 2);
	int arg = -1;
	if (!lua_isnil(L, 3)) {
		arg = (int)lua_tonumber(L, 2);
	}

	lua_currentInstance->addLuaTimer(duration, funcName, arg);

	return 0;

}

void Game::addLuaTimer(float duration, string funcName, int arg) {

	luaTimer t;
	t.duration = duration;
	t.funcName = funcName;
	t.arg = arg;
	t.id = nextTimerId();

	timers.push_back(t);

	_bridgeInterface->addTimer(t.duration, t.id);

}

void Game::timerCallback(int timerId) {

	// TODO: might not be lua timer...
	vector<luaTimer>::iterator it;
	for (it = timers.begin(); it != timers.end(); it++) {
		if (it->id == timerId) {
			luaTimer t = *it;
			lua_getglobal(_rules, t.funcName.c_str());
			// TODO: support all sorts of args...
			if (t.arg != -1) {
				lua_pushnumber(_rules, t.arg);
				lua_call(_rules, 1, 0);
			} else {
				lua_call(_rules, 0, 0);
			}
			timers.erase(it);
			break;
		}
	}

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

		lua_pushcfunction(L, lua_addTimer);
		lua_setglobal(L, "addTimer");

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
	_physics->resetBallsToInitialPosition();
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

