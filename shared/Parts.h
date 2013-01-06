
#ifndef __PINBALL_PARTS__
#define __PINBALL_PARTS__

#include <string>
#include <vector>
#include <map>
#include <stdint.h>

#include "Types.h"

typedef struct Material {
	std::string n;
	float e;
	float f;
	float d;
} Material;
typedef std::map<std::string, Material>::iterator it_Material;

typedef struct Texture {
	std::string n;
	std::string filename;
	uint32_t gl_index;
	int w;
	int h;
} Texture;
typedef std::map<std::string, Texture>::iterator it_Texture;

typedef struct TextureInfo {
	Texture *t;
	int x;
	int y;
	int w;
	int h;
	float a;
} TextureInfo;

typedef struct Part {
	std::string n;
	int v;
	std::string s;
	float r1;
	float r2;
	Material *m;
	TextureInfo t;
} Part;
typedef std::map<std::string, Part>::iterator it_Part;

typedef struct LayoutItem {
	std::string n;
	Part *o;
	std::vector<Coord2> v;
	float s;
	int count;
	std::vector<Body *> bodies;
	std::vector<Shape *> shapes;
	float width;
	float height;
	bool editing;
	Coord2 c;
} LayoutItem;
typedef std::map<std::string, LayoutItem>::iterator it_LayoutItem;

struct Overlay {
	std::string n;
	std::string t;
	std::string l;
	std::string v;
	std::string x;
	Coord2 p;
	std::string a;
	float s;
	float o;
};
typedef std::map<std::string, Overlay>::iterator it_Overlay;

#endif

