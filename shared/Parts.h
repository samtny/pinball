#ifndef __PARTS__
#define __PARTS__

using namespace std;
#include <string>
#include <map>

#include "chipmunk/chipmunk.h"

typedef struct Coord2{float x,y;} Coord2;

typedef struct materialProperties {
	string n;
	float e;
	float f;
	float d;
} materialProperties;
typedef map<string, materialProperties>::iterator it_materialProperties;

typedef struct objectTextureProperties {
	string n;
	int x;
	int y;
	int w;
	int h;
	float a;
} objectTextureProperties;

typedef struct objectProperties {
	string n;
	string s;
	float r1;
	float r2;
	materialProperties m;
	objectTextureProperties t;
} objectProperties;
typedef map<string, objectProperties>::iterator it_objectProps;

typedef struct layoutItem {
	string n;
	objectProperties o;
	cpVect v[200];
	float s;
	int count;
	cpBody *body;
	float width;
	float height;
} layoutItem;
typedef map<string, layoutItem>::iterator it_layoutItems;

typedef struct textureProperties {
	string name;
	string filename;
	uint gl_index;
	int w;
	int h;
} textureProperties;
typedef map<string, textureProperties>::iterator it_textureProperties;

struct overlayProperties {
	string n;
	string t;
	string l;
	string v;
	string x;
	Coord2 p;
	string a;
	float s;
	float o;
};
typedef map<string, overlayProperties>::iterator it_overlayProperties;

#endif