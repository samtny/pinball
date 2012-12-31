#include "Editor.h"

#include "chipmunk/chipmunk.h"

#include "PinballBridgeInterface.h"

#include "Game.h"

#include "Physics.h"

#include "Camera.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <iostream>
#include <fstream>

static Editor *_editorCurrentInstance;

static float _scale = 37;

Editor::Editor(void) {
	_editorCurrentInstance = this;
	_state.editMode = EDIT_MODE_NONE;
	_state.selectionStart.x = 0;
	_state.selectionStart.y = 0;
	_state.selectionEnd.x = 0;
	_state.selectionEnd.y = 0;
	_currentEditObjectName = 0;
}

Editor::~Editor(void) {

}

void Editor::init() {
	this->loadConfig();
	this->loadMaterials();
	this->loadObjects();
}

void Editor::setBridgeInterface(PinballBridgeInterface *bridgeInterface) {
	_bridgeInterface = bridgeInterface;
}

void Editor::setGame(Game *game) {
	_game = game;
}

void Editor::setPhysics(Physics *physics) {
	_physics = physics;
	// TODO: this is a small hack...
	this->pushState();
}

void Editor::setCamera(Camera *camera) {
	_camera = camera;
}

const EditorState *Editor::getState() {
	return &_state;
}

void Editor::setState(EditorState state) {

	_state = state;

	switch (_state.editMode) {
	case EDIT_MODE_SELECT:
	case EDIT_MODE_SELECT_EXCLUSIVE:
	case EDIT_MODE_SELECT_MANY:
		selectItems();
		break;
	case EDIT_MODE_MOVE:
	case EDIT_MODE_MOVE_BEGIN:
	case EDIT_MODE_MOVE_COMMIT:
		moveItems();
		break;
	case EDIT_MODE_ROTATE:
	case EDIT_MODE_ROTATE_COMMIT:
		rotateItems();
		break;
	case EDIT_MODE_INSERT_BEGIN:
		insertItems();
		break;
	case EDIT_MODE_DUPE:
		dupeItems();
		break;
	default:
		break;
	}
	
}

const EditObject *Editor::getCurrentEditObject() {
	return &_currentEditObject;
}

void Editor::dupeItems() {

	map<string, layoutItem> *items = _physics->getLayoutItems();

	map<string, layoutItem> toDupe;

	float minOffset = 65535;

	for (it_layoutItems it = items->begin(); it != items->end(); it++) {

		layoutItem item = it->second;

		if (item.editing == true) {
			
			toDupe[item.n] = item;
			layoutItem *orig = &(&*it)->second;
			orig->editing = false;

			float offset = abs(item.v[0].x - item.v[item.count-1].x);

			if (offset == 0) {
				offset = item.o.r1 * 2;
			}

			if (offset < minOffset) {
				minOffset = offset;
			}

		}

	}

	if (toDupe.size() > 0) this->pushState();

	for (it_layoutItems it = toDupe.begin(); it != toDupe.end(); it++) {
	
		layoutItem item = it->second;

		if (item.editing == true) {

			for (int i = 0; i < item.count; i++) {
				item.v[i].x += minOffset;
			}

			// rename;
			char num[21];
			sprintf(num, "%d", _currentEditObjectName);
			item.n = "_" + item.o.n + num;
			_currentEditObjectName++;

			item.editing = true;

			_physics->addLayoutItem(item);

		}

	}

}

void Editor::deleteItems() {

	map<string, layoutItem> *items = _physics->getLayoutItems();
	vector<string> toRemove;

	layoutItem box;

	bool found = false;

	for (it_layoutItems it = items->begin(); it != items->end(); it++) {

		layoutItem item = it->second;

		if (item.editing == true && strcmp(item.n.c_str(), "box") != 0) {

			if (found == false) {
				this->pushState();
				found == true;
			}

			_physics->destroyObject(&item);
			toRemove.push_back(item.n);

		} else {
			box = item;
		}

	}

	if (box.editing == true && box.n == "box") {

		if (found == false) {
			this->pushState();
			found == true;
		}

		_physics->destroyObject(&box);
		toRemove.push_back(box.n);
	}

	for (int i = 0; i < toRemove.size(); i++) {

		items->erase(items->find(toRemove[i]));

	}

}

void Editor::undo() {

	// TODO: this is a complete hack job...
	if (_history.size() > 1) {

		map<string, layoutItem> *items = _physics->getLayoutItems();

		layoutItem box;

		for (it_layoutItems it = items->begin(); it != items->end(); it++) {

			layoutItem item = it->second;

			if (strcmp(item.n.c_str(), "box") != 0) {

				_physics->destroyObject(&item);

			} else {
				box = item;
			}

		}

		if (box.n == "box") {
			_physics->destroyObject(&box);
		}

		items->clear();
		
		_history.pop_back();

		EditorState *state = &_history.back();

		layoutItem *newBox = &state->items["box"];

		_physics->createObject(newBox);
		items->insert(make_pair("box", *newBox));

		for (it_layoutItems it = state->items.begin(); it != state->items.end(); it++) {

			layoutItem *item = &(&*it)->second;

			if (strcmp(item->n.c_str(), "box") != 0) {
				_physics->createObject(item);
				items->insert(make_pair(item->n, *item));
			}

		}

	}

}

void Editor::save() {

	map<string, layoutItem> *items = _physics->getLayoutItems();

	const char *savePath = _bridgeInterface->getPathForScriptFileName("user.layout.lua");

	ofstream layout;
	layout.open(savePath);
	layout.precision(6);

	layout << "layout = {\n";

	int count = 0;
	for (it_layoutItems it = items->begin(); it != items->end(); it++) {

		layoutItem item = it->second;

		if (count > 0) layout << ",\n";

		layout << "\t" << item.n << " = {\n";

		layout << "\t\t" << "o = \"" << item.o.n << "\"";

		if (item.count > 0) {

			layout << ",\n";

			layout << "\t\t" << "v = {";

			for (int i = 0; i < item.count; i++) {

				if (i > 0) layout << " ,";

				layout << " {" << item.v[i].x * _scale;
				layout << ",";
				layout << item.v[i].y * _scale;
				layout << "}";

			}
			
			layout << " }";

		}

		if (item.s != 1) {
			layout << ",\n";
			layout << "\t\t" << "s = " << item.s << "\n";
		} else {
			layout << "\n";
		}

		layout << "\t}";

		count++;

	}

	layout << "\n}\n";

	layout.close();

}

void Editor::load() {

	// TODO: not be a complete hack job

	map<string, layoutItem> *items = _physics->getLayoutItems();

	layoutItem box;

	for (it_layoutItems it = items->begin(); it != items->end(); it++) {

		layoutItem item = it->second;

		if (strcmp(item.n.c_str(), "box") != 0) {

			_physics->destroyObject(&item);

		} else {
			box = item;
		}

	}

	if (box.n == "box") {
		_physics->destroyObject(&box);
	}

	items->clear();

	_layout.clear();
	this->loadLayout();



	layoutItem *newBox = &_layout["box"];

	_physics->applyScale(newBox);
	_physics->createObject(newBox);

	items->insert(make_pair("box", *newBox));

	for (it_layoutItems it = _layout.begin(); it != _layout.end(); it++) {

		layoutItem *item = &(&*it)->second;

		if (strcmp(item->n.c_str(), "box") != 0) {
			_physics->applyScale(item);
			_physics->createObject(item);
			items->insert(make_pair(item->n, *item));
		}

	}

}

void Editor::pushState() {

	// add current editor state to history;
	map<string, layoutItem> items = *_physics->getLayoutItems();
	_state.items = items;
	_history.push_back(_state);

}

void Editor::insertItems() {

	switch (_state.editMode) {
	case EDIT_MODE_INSERT_BEGIN:
		{
			EditObject obj;
			obj.object = _objects[_state.editObjectName];
			obj.vCurrent = 0;
			_currentEditObject = obj;
		}
		break;
	case EDIT_MODE_INSERT:
		{
			Coord2 v = { _state.selectionStart.x, _state.selectionStart.y };
			_currentEditObject.verts[_currentEditObject.vCurrent] = _camera->transform(v);
			_currentEditObject.vCurrent++;

			if (_currentEditObject.vCurrent == _currentEditObject.object.v) {
				
				this->pushState();

				layoutItem l;
				l.o = _currentEditObject.object;

				l.s = 1;

				float localScale = _scale * 1 / l.s;

				l.o.r1 *= 1 / localScale;
				l.o.r2 *= 1 / localScale;

				char num[21];
				sprintf(num, "%d", _currentEditObjectName);
				l.n = "_" + _currentEditObject.object.n + num;
				_currentEditObjectName++;

				l.count = _currentEditObject.object.v;
				l.editing = false;
				
				for (int i = 0; i < _currentEditObject.object.v; i++) {
					l.v[i].x = _currentEditObject.verts[i].x;
					l.v[i].y = _currentEditObject.verts[i].y;
				}
				
				_physics->addLayoutItem(l);
				
				// reset;
				_currentEditObject.vCurrent = 0;

			}

		}
		break;
	default:
		break;
	}

}

void Editor::loadConfig() {

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *configFileName = _bridgeInterface->getPathForScriptFileName((void *)"config.lua");

	int error = luaL_dofile(L, configFileName);
	if (!error) {

        lua_getglobal(L, "config");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				const char *key = lua_tostring(L, -2);
                    
				if (strcmp("scale", key) == 0) {

					_scale = (float)lua_tonumber(L, -1);

				}
				   
				lua_pop(L, 1);
			}
            
		}
        
		lua_pop(L, 1); // pop table

    } else {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  // pop err from lua stack
	}

	lua_close(L);

}

void Editor::loadMaterials() {
	
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *materialsFileName = _bridgeInterface->getPathForScriptFileName((void *)"materials.lua");

	int error = luaL_dofile(L, materialsFileName);
	if (!error) {

        lua_getglobal(L, "materials");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				// key;
				const char *name = lua_tostring(L, -2);

				materialProperties props = { "", -1, -1, -1 };

				// "value" is properties table;
				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {

					// property name
					const char *key = lua_tostring(L, -2);

					// property value
					float val = (float)lua_tonumber(L, -1);

					if (strcmp("e", key) == 0) {
						props.e = val;
					} else if (strcmp("f", key) == 0) {
						props.f = val;
					} else if (strcmp("d", key) == 0) {
						props.d = val;
					}

					lua_pop(L, 1);
				}
				
				_materials.insert(make_pair(name, props));

				lua_pop(L, 1);
			}

		}

		lua_pop(L, 1); // pop materials table

    } else {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  // pop err from lua stack
	}

	lua_close(L);

}


void Editor::loadObjects() {

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *objectsPath = _bridgeInterface->getPathForScriptFileName((void *)"objects.lua");

	int error = luaL_dofile(L, objectsPath);
	if (!error) {

        lua_getglobal(L, "objects");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				// key;
				const char *name = lua_tostring(L, -2);

				objectProperties props = { name, -1, "", -1, -1, { "", -1, -1, -1, }, { "", -1, -1, -1, -1 } };

				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {
					
					const char *key = lua_tostring(L, -2);

					if (strcmp("s", key) == 0) {
						props.s = lua_tostring(L, -1);
					} else if (strcmp("v", key) == 0) {
						props.v = (int)lua_tonumber(L, -1);
					} else if (strcmp("m", key) == 0) {
						props.m = _materials[lua_tostring(L, -1)];
					} else if (strcmp("r1", key) == 0) {
						props.r1 = (float)lua_tonumber(L, -1);
					} else if (strcmp("r2", key) == 0) {
						props.r2 = (float)lua_tonumber(L, -1);
					} else if (strcmp("t", key) == 0) {

						lua_pushnil(L);
						while(lua_next(L, -2) != 0) {
							
							const char *tkey = lua_tostring(L, -2);

							if (strcmp("n", tkey) == 0) {
								props.t.n = lua_tostring(L, -1);
							} else if (strcmp("x", tkey) == 0) {
								props.t.x = (int)lua_tonumber(L, -1);
							} else if (strcmp("y", tkey) == 0) {
								props.t.y = (int)lua_tonumber(L, -1);
							} else if (strcmp("w", tkey) == 0) {
								props.t.w = (int)lua_tonumber(L, -1);
							} else if (strcmp("h", tkey) == 0) {
								props.t.h = (int)lua_tonumber(L, -1);
							} else if (strcmp("a", tkey) == 0) {
								props.t.a = (float)lua_tonumber(L, -1);
							}

							lua_pop(L, 1);

						}

					}

					lua_pop(L, 1);
				}
				
				_objects.insert(make_pair(name, props));

				lua_pop(L, 1);
			}

		}

		lua_pop(L, 1); // pop table

    } else {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  // pop err from lua stack
	}

	lua_close(L);

}

void Editor::loadLayout() {
	
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *layoutPath = _bridgeInterface->getPathForScriptFileName((void *)"layout.lua");

	int error = luaL_dofile(L, layoutPath);
	if (!error) {

        lua_getglobal(L, "layout");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				// key;
				const char *name = lua_tostring(L, -2);

				layoutItem props = { name };
				props.s = -1;
				props.editing = false;

				lua_pushnil(L);
				while(lua_next(L, -2) != 0) {
					
					const char *key = lua_tostring(L, -2);

					if (strcmp("o", key) == 0) {
						
						props.o = _objects[lua_tostring(L, -1)];

					} else if (strcmp("v", key) == 0) {
						
						int length = lua_rawlen(L, -1);
						
						// traverse 2d vects
						for (int i = 1; i <= length; i++)
						{

							// init vect object
							cpVect v;

							// get the 2d table
							lua_rawgeti(L, -1, i);

							// get the first vertex
							lua_rawgeti(L, -1, 1);
							v.x = (float)lua_tonumber(L, -1);
							lua_pop(L, 1);

							// get the second vertex
							lua_rawgeti(L, -1, 2);
							v.y = (float)lua_tonumber(L, -1);
							lua_pop(L, 1);
							
							// pop the table;
							lua_pop(L, 1);
							
							// assign vect to array
							props.v[i-1] = v;

						}

						props.count = length;

					} else if (strcmp("s", key) == 0) {
						props.s = (float)lua_tonumber(L, -1);
					}

					lua_pop(L, 1);
				}
				
				if (props.s == -1) {
					props.s = 1;
				}

				_layout.insert(make_pair(name, props));

				lua_pop(L, 1);
			}

		}

		lua_pop(L, 1); // pop table

    } else {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  // pop err from lua stack
	}

	lua_close(L);

}

void Editor::selectItems() {

	map<string, layoutItem> *items = _physics->getLayoutItems();

	Coord2 start = _camera->transform(_state.selectionStart);
	Coord2 end = _camera->transform(_state.selectionEnd);

	for (it_layoutItems it = items->begin(); it != items->end(); it++) {
	
		layoutItem *item = &(&*it)->second;

		
		bool inside = false;

		for (int i = 0; i < item->count; i++) {
			cpVect v = item->v[i];
			if (
				v.x >= min(start.x, end.x) &&
				v.x <= max(start.x, end.x) &&
				v.y >= min(start.y, end.y) &&
				v.y <= max(start.y, end.y)
				) {
				inside = true;
				break;
			}
		}

		if (inside == true) {
			item->editing = true;
		} else if (_state.editMode == EDIT_MODE_SELECT_EXCLUSIVE) {
			item->editing = false;
		}

	}

}

vector<string> Editor::getObjectNames() {

	vector<string> names;

	for (it_objectProps it = _objects.begin(); it != _objects.end(); it++) {

		objectProperties object = it->second;

		names.push_back(object.n);

	}

	return names;

}

void Editor::moveItems() {

	if (
		_state.editMode == EDIT_MODE_MOVE_COMMIT &&
		(_state.selectionStart.x != _state.selectionEnd.x  ||
		_state.selectionStart.x != _state.selectionEnd.y)
		) 
	{

		this->pushState();

		map<string, layoutItem> *items = _physics->getLayoutItems();

		Coord2 start = _camera->transform(_state.selectionStart);
		Coord2 end = _camera->transform(_state.selectionEnd);

		for (it_layoutItems it = items->begin(); it != items->end(); it++) {
	
			layoutItem *item = &(&*it)->second;

			if (item->editing == true) {

				_physics->destroyObject(item);

				for (int i = 0; i < item->count; i++) {
					item->v[i].x = item->v[i].x - (start.x - end.x);
					item->v[i].y = item->v[i].y - (start.y - end.y);
				}

				_physics->createObject(item);

			}

		}

	}

}

void Editor::rotateItems() {
	
	if (
		_state.editMode == EDIT_MODE_ROTATE_COMMIT &&
		(_state.selectionStart.x != _state.selectionEnd.x ||
		_state.selectionStart.y != _state.selectionEnd.y)
		) 
	{

		this->pushState();

		map<string, layoutItem> *items = _physics->getLayoutItems();

		Coord2 start = _state.selectionStart;
		Coord2 end = _state.selectionEnd;

		for (it_layoutItems it = items->begin(); it != items->end(); it++) {
			
			layoutItem *item = &(&*it)->second;

			if (item->editing == true) {

				_physics->destroyObject(item);

				// find center
				Coord2 c = { 0, 0 };
				for (int i = 0; i < item->count; i++) {
					Coord2 t = { item->v[i].x, item->v[i].y };
					c = coordadd(c, t);
				}
				c = coordmult(c, 1 / (float)item->count);
				
				// mouse position
				Coord2 m = _camera->transform(end);

				// rotvec
				Coord2 rotvec = coordsub(m, c);

				// rot
				float rot = atan2f(rotvec.y, rotvec.x) * (180.0f / M_PI);

				// rotate all points around c
				for (int i = 0; i < item->count; i++) {
					
					// current vertex
					Coord2 v = { item->v[i].x, item->v[i].y };

					// relative to center
					Coord2 vrel = coordsub(v, c);
					
					if (vrel.x != 0 || vrel.y != 0) {

						// normalized
						Coord2 vreln = coordnormalize(vrel);
						
						// transform normalized vector by rotvect
						Coord2 vrelnt = coordrotate(vreln, rotvec);
						
						// normalize result
						Coord2 vrelntn = coordnormalize(vrelnt);
						
						// multiply by length of original center-relative vector
						Coord2 vt = coordmult(vrelntn, coordlen(vrel));

						// add back center
						Coord2 vfinal = coordadd(c, vt);

						item->v[i].x = vfinal.x;
						item->v[i].y = vfinal.y;

					}

				}

				_physics->createObject(item);

			}

		}

	}
	
}

