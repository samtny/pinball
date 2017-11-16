
#ifndef __PINBALL_TYPES__
#define __PINBALL_TYPES__

#include <map>
#include <math.h>
#include <stdint.h>
#include <string>
#include <vector>

#include "chipmunk/chipmunk.h"

typedef cpBody Body;
typedef cpShape Shape;

typedef struct Coord2{
	cpFloat x,y;
	operator cpVect() {
		cpVect v;
		v.x = x;
		v.y = y;
		return v;
	};
} Coord2;
/// Convenience constructor for Coord2 structs.
static inline Coord2 coord(const cpFloat x, const cpFloat y)
{
	Coord2 v = {x, y};
	return v;
}
/// Add two vectors.
static inline Coord2 coordadd(const Coord2 v1, const Coord2 v2) {
	return coord(v1.x + v2.x, v1.y + v2.y);
}
/// Subtract two vectors.
static inline Coord2 coordsub(const Coord2 v1, const Coord2 v2)
{
	return coord(v1.x - v2.x, v1.y - v2.y);
}
/// Scalar multiplication.
static inline Coord2 coordmult(const Coord2 v, const float s)
{
	return coord(v.x*s, v.y*s);
}
/// Vector length
static inline cpFloat coordlen(const Coord2 v) {
	return sqrt(v.x * v.x + v.y * v.y);
}
/// Rotate vector 1 by vector 2
static inline Coord2 coordrotate(const Coord2 v1, const Coord2 v2) {
	return coord(v1.x * v2.x - v1.y * v2.y, v1.x * v2.y + v1.y * v2.x);
}
/// Normalize vector
static inline Coord2 coordnormalize(const Coord2 v) {
	return coordmult(v, 1.0f/(float)coordlen(v));
}


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

enum CollisionType {
    CollisionTypeNone,
    CollisionTypeSwitch,
    CollisionTypeBall,
    CollisionTypeTarget,
    CollisionTypeTargetSwitch,
    CollisionTypePopbumper,
    CollisionTypeSlingshot,
    CollisionTypeSlingshotSwitch,
    CollisionTypeKraken
};

enum shapeGroup {
    shapeGroupBox,
    shapeGroupBall,
    shapeGroupSwitch,
    shapeGroupFlippers,
    shapeGroupTargets,
    shapeGroupPopbumpers,
    shapeGroupSlingshots,
    shapeGroupKraken
};

#endif

