#include "Editor.h"

#include "PinballBridgeInterface.h"

#include "Playfield.h"

#include "Parts.h"

#include "Game.h"

#include "Physics.h"

#include "Camera.h"

#include "chipmunk/chipmunk.h"

extern "C" {
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}

#include <iostream>
#include <fstream>

using std::string;
using std::map;
using std::vector;
using std::make_pair;
using std::ofstream;
using std::min;
using std::max;

Editor::Editor(void) {
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
}

void Editor::setBridgeInterface(PinballBridgeInterface *bridgeInterface) {
	_bridgeInterface = bridgeInterface;
}

void Editor::setPlayfield(Playfield *playfield) {
	_playfield = playfield;
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
	case EDIT_MODE_PAN:
		break;
	case EDIT_MODE_PAN_COMMIT:
		commitPan();
		break;
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

	map<string, LayoutItem> *items = _playfield->getLayout();

	map<string, LayoutItem> toDupe;

	float minOffset = 65535;

	for (it_LayoutItem it = items->begin(); it != items->end(); it++) {

		LayoutItem item = it->second;

		if (item.editing == true) {
			
			toDupe[item.n] = item;
			LayoutItem *orig = &(&*it)->second;
			orig->editing = false;

			float offset = (float)abs(item.v[0].x - item.v[item.o->count-1].x);

			if (offset == 0) {
				offset = item.o->r1 * 2;
			}

			if (offset < minOffset) {
				minOffset = offset;
			}

		}

	}

	if (toDupe.size() > 0) this->pushState();

	for (it_LayoutItem it = toDupe.begin(); it != toDupe.end(); it++) {
	
		LayoutItem item = it->second;
		LayoutItem newItem;

		newItem.c = item.c;
		newItem.height = item.height;
		newItem.o = item.o;
		newItem.s = item.s;
		newItem.v = item.v;
		newItem.width = item.width;

		if (item.editing == true) {

			for (int i = 0; i < item.o->count; i++) {
				newItem.v[i].x += minOffset;
			}

			// rename;
			char num[21];
			sprintf(num, "%d", _currentEditObjectName);
			newItem.n = "_" + newItem.o->n + num;
			_currentEditObjectName++;
			
			newItem.editing = true;

			//_physics->addLayoutItem(item);
			_playfield->getLayout()->insert(make_pair(newItem.n, newItem));

			_physics->createObject(&_playfield->getLayout()->find(newItem.n)->second);

		}

	}

}

void Editor::deleteItems() {

	map<string, LayoutItem> *items = _playfield->getLayout();
	vector<string> toRemove;

	LayoutItem box;

	bool found = false;

	for (it_LayoutItem it = items->begin(); it != items->end(); it++) {

		LayoutItem item = it->second;

		if (item.editing == true && strcmp(item.n.c_str(), "box") != 0) {

			if (found == false) {
				this->pushState();
				found = true;
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
			found = true;
		}

		_physics->destroyObject(&box);
		toRemove.push_back(box.n);
	}

	for (int i = 0; i < (int)toRemove.size(); i++) {

		items->erase(items->find(toRemove[i]));

	}

}

void Editor::undo() {

	// kill current
	for (it_LayoutItem it = _playfield->getLayout()->begin(); it != _playfield->getLayout()->end(); it++) {
		LayoutItem *item = &(&*it)->second;
		item->editing = true;
	}
	this->deleteItems();

	// TODO: borked

	_history.pop_back();

	EditorState state = _history.back();

	for (it_LayoutItem it = state.items.begin(); it != state.items.end(); it++) {

		LayoutItem *item = &(&*it)->second;

		item->bodies.clear();
		item->shapes.clear();

		_physics->createObject(item);

		_playfield->getLayout()->insert(make_pair(item->n, *item));

	}

}

void Editor::save() {

	map<string, LayoutItem> *items = _playfield->getLayout();

	const char *savePath = _bridgeInterface->getScriptPath((const char *)"user.layout.lua");

	ofstream layout;
	layout.open(savePath);
	layout.precision(6);

	layout << "layout = {\n";

	int count = 0;
	for (it_LayoutItem it = items->begin(); it != items->end(); it++) {

		LayoutItem item = it->second;

		if (count > 0) layout << ",\n";

		layout << "\t" << item.n << " = {\n";

		layout << "\t\t" << "o = \"" << item.o->n << "\"";

		if (item.o->count > 0) {

			layout << ",\n";

			layout << "\t\t" << "v = {";

			for (int i = 0; i < item.o->count; i++) {

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

	for (it_LayoutItem it = _playfield->getLayout()->begin(); it != _playfield->getLayout()->end(); it++) {

		LayoutItem *item = &(&*it)->second;
		item->editing = true;

	}

	this->deleteItems();

	_playfield->reload();

	_physics->createObjects();

}

void Editor::pushState() {

	// add current editor state to history;
	map<string, LayoutItem> items = *_playfield->getLayout();
	_state.items = items;
	_history.push_back(_state);

}

void Editor::insertItems() {

	switch (_state.editMode) {
	case EDIT_MODE_INSERT_BEGIN:
		{
			EditObject obj;
			obj.part = &_playfield->getParts()->find(_state.editObjectName)->second;
			obj.vCurrent = 0;
			_currentEditObject = obj;
		}
		break;
	case EDIT_MODE_INSERT:
		{
			Coord2 v = { _state.selectionStart.x, _state.selectionStart.y };
			_currentEditObject.verts.push_back(_camera->transform(v));
			_currentEditObject.vCurrent++;

			if (_currentEditObject.vCurrent == _currentEditObject.part->count) {
				
				this->pushState();

				LayoutItem l;
				l.o = _currentEditObject.part;

				l.s = 1;

				//double localScale = _scale * 1 / l.s;

				//l.o->r1 *= 1 / (float)localScale;
				//l.o->r2 *= 1 / (float)localScale;

				char num[21];
				sprintf(num, "%d", _currentEditObjectName);
				l.n = "_" + _currentEditObject.part->n + num;
				_currentEditObjectName++;

				//l.o->count = _currentEditObject.object->count;
				l.editing = false;
				
				for (int i = 0; i < _currentEditObject.part->count; i++) {
					Coord2 v = coord(_currentEditObject.verts[i].x, _currentEditObject.verts[i].y);
					l.v.push_back(v);
				}
				
				_playfield->getLayout()->insert(make_pair(l.n, l));
				
				_physics->createObject(&_playfield->getLayout()->find(l.n)->second);

				// reset;
				_currentEditObject.vCurrent = 0;
				_currentEditObject.verts.clear();

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

	const char *configFileName = _bridgeInterface->getScriptPath((const char *)"config.lua");

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

void Editor::selectItems() {

	map<string, LayoutItem> *items = _playfield->getLayout();

	Coord2 start = _camera->transform(_state.selectionStart);
	Coord2 end = _camera->transform(_state.selectionEnd);

	for (it_LayoutItem it = items->begin(); it != items->end(); it++) {
	
		LayoutItem *item = &(&*it)->second;

		
		bool inside = false;

		for (int i = 0; i < item->o->count; i++) {
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

	for (it_Part it = _playfield->getParts()->begin(); it != _playfield->getParts()->end(); it++) {

		Part object = it->second;

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

		map<string, LayoutItem> *items = _playfield->getLayout();

		Coord2 start = _camera->transform(_state.selectionStart);
		Coord2 end = _camera->transform(_state.selectionEnd);

		for (it_LayoutItem it = items->begin(); it != items->end(); it++) {
	
			LayoutItem *item = &(&*it)->second;

			if (item->editing == true) {

				_physics->destroyObject(item);

				for (int i = 0; i < item->o->count; i++) {
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

		map<string, LayoutItem> *items = _playfield->getLayout();

		//Coord2 start = _state.selectionStart;
		Coord2 end = _state.selectionEnd;

		for (it_LayoutItem it = items->begin(); it != items->end(); it++) {
			
			LayoutItem *item = &(&*it)->second;

			if (item->editing == true) {

				_physics->destroyObject(item);

				// find center
				Coord2 c = { 0, 0 };
				for (int i = 0; i < item->o->count; i++) {
					Coord2 t = { item->v[i].x, item->v[i].y };
					c = coordadd(c, t);
				}
				c = coordmult(c, 1 / (float)item->o->count);
				
				// mouse position
				Coord2 m = _camera->transform(end);

				// rotvec
				Coord2 rotvec = coordsub(m, c);

				// rot
				//double rot = atan2f((float)rotvec.y, (float)rotvec.x) * (180.0f / M_PI);

				// rotate all points around c
				for (int i = 0; i < item->o->count; i++) {
					
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
						Coord2 vt = coordmult(vrelntn, (float)coordlen(vrel));

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

void Editor::commitPan() {

	_camera->setPan(coordadd(_camera->getPan(), coordsub(_state.selectionStart, _state.selectionEnd)));

}

