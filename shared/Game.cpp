
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
	_paused = false;
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
	
	lua_getglobal(_rules, "resetAll");
	lua_call(_rules, 0, 0);

}

static int lua_resetBallPosition(lua_State *L) {

	lua_currentInstance->resetBallPosition();
	return 0;

}

static int lua_setCameraMode(lua_State *L) {

	int count = lua_gettop(L);

	if (count == 1) {

		string modeName = lua_tostring(L, 1);
		lua_currentInstance->setCameraMode(modeName.c_str());

	}

	return 0;

}

struct luaTimer {
	int id;
	float duration;
	string funcName;
	int arg;
};

static int sNextTimerId = 0;
static int nextTimerId() {
	return ++sNextTimerId;
}

static vector<luaTimer> timers;

static int lua_addTimer(lua_State *L) {

    // TODO: consider incoming arguments;
	//int count = lua_gettop(L);
    
	float duration = (float)lua_tonumber(L, 1);
	string funcName = lua_tostring(L, 2);
	int arg = -1;
	if (!lua_isnil(L, 3)) {
		arg = (int)lua_tonumber(L, 2);
	}

	lua_currentInstance->addLuaTimer(duration, funcName, arg);

	return 0;

}

static int lua_playSound(lua_State *L) {
    
    // TODO: something...
    
    return 0;
    
}

static int lua_updateOverlayText(lua_State *L) {

	int count = lua_gettop(L);

	if (count == 2) {
		const char *key = lua_tostring(L, 1);
		const char *val = lua_tostring(L, 2); // changes lua stack value to string, btw...
		lua_currentInstance->updateOverlayText(key, val);
	}

	return 0;

}

static int lua_doCameraEffect(lua_State *L) {

	int count = lua_gettop(L);

	if (count == 1) {
		const char *effectName = lua_tostring(L, 1);
		lua_currentInstance->doCameraEffect(effectName);
	}

	return 0;
}

static int lua_activateMech(lua_State *L) {

	int count = lua_gettop(L);

	if (count == 1) {
		const char *mechName = lua_tostring(L, 1);
		lua_currentInstance->activateMech(mechName);
	}

	return 0;

}

static int lua_deactivateMech(lua_State *L) {

	int count = lua_gettop(L);

	if (count == 1) {
		const char *mechName = lua_tostring(L, 1);
		lua_currentInstance->deactivateMech(mechName);
	}

	return 0;

}

void Game::deactivateMech(const char *mechName) {
	
	_physics->deactivateMech(mechName);

}

void Game::activateMech(const char *mechName) {

	_physics->activateMech(mechName);

}

void Game::doCameraEffect(const char *effectName) {

	_renderer->doCameraEffect(effectName);

}

void Game::updateOverlayText(const char *key, const char *val) {

	_renderer->setOverlayText(key, val);

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

		lua_pushcfunction(L, lua_setCameraMode);
		lua_setglobal(L, "setCameraMode");

		lua_pushcfunction(L, lua_addTimer);
		lua_setglobal(L, "addTimer");

		lua_pushcfunction(L, lua_updateOverlayText);
		lua_setglobal(L, "updateOverlayText");

		lua_pushcfunction(L, lua_doCameraEffect);
		lua_setglobal(L, "doCameraEffect");

		lua_pushcfunction(L, lua_activateMech);
		lua_setglobal(L, "activateMech");

		lua_pushcfunction(L, lua_deactivateMech);
		lua_setglobal(L, "deactivateMech");

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

void Game::switchOpened(const char *switchName) {
	lua_getglobal(_rules, "handleSwitchOpened");
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

void Game::setCameraMode(const char *modeName) {

	fprintf(stderr, "%s\n", "setCameraMode");
	_renderer->setCameraMode(modeName);	

}

void Game::setZoomLevel(float zoomLevel) {
	_renderer->setZoomLevel(zoomLevel);
}

float Game::getZoomLevel() {
	return _renderer->getZoomLevel();
}

void Game::setPaused(bool paused) {
	_paused = paused;
	_physics->setPaused(_paused);
}

bool Game::getPaused() {
	return _paused;
}

const char *Game::getGameName() {
	return _bridgeInterface->getGameName();
}