
#ifndef __PINBALL_PARTS__
#define __PINBALL_PARTS__

using namespace std;
#include <string>
#include <vector>
#include <map>
#include <stdint.h>

#include "Types.h"

typedef struct Material {
	string n;
	float e;
	float f;
	float d;
} Material;
typedef map<string, Material>::iterator it_Material;

typedef struct Texture {
	string n;
	string filename;
	uint32_t gl_index;
	int w;
	int h;
} Texture;

typedef struct TextureInfo {
	Texture *t;
	int x;
	int y;
	int w;
	int h;
	float a;
} TextureInfo;

typedef struct Part {
	string n;
	int v;
	string s;
	float r1;
	float r2;
	Material *m;
	TextureInfo t;
} Part;
typedef map<string, Part>::iterator it_Part;

typedef struct LayoutItem {
	string n;
	Part *o;
	vector<Coord2> v;
	float s;
	int count;
	vector<Body *> bodies;
	vector<Shape *> shapes;
	float width;
	float height;
	bool editing;
	Coord2 c;
} LayoutItem;
typedef map<string, LayoutItem>::iterator it_LayoutItem;

struct Overlay {
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
typedef map<string, Overlay>::iterator it_Overlay;

#endif