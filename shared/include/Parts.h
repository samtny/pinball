
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
	float x;
	float y;
	float a;
} TextureInfo;
#define TEXTUREINFO_INIT { NULL, -1, -1, 0 }

typedef std::map<std::string, std::string> PartMeta;

typedef struct Part {
	std::string n;
	int count;
	std::string s;
	float r1;
	float r2;
	Material *m;
	TextureInfo t;
    PartMeta meta;
} Part;
typedef std::map<std::string, Part>::iterator it_Part;
#define PART_INIT { "", -1, "", -1, -1, NULL, TEXTUREINFO_INIT }

typedef struct LayoutItem {
	std::string n;
	Part *o;
	std::vector<Coord2> v;
	int count;
	float s;
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
	Texture* x;
	Coord2 p;
	std::string a;
	float s;
	float o;
};
typedef std::map<std::string, Overlay>::iterator it_Overlay;

#endif

