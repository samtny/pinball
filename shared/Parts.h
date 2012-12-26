#ifndef __PARTS__
#define __PARTS__

using namespace std;
#include <string>
#include <vector>
#include <map>

#include "chipmunk/chipmunk.h"

typedef struct Coord2{float x,y;} Coord2;
/// Convenience constructor for Coord2 structs.
static inline Coord2 coord(const float x, const float y)
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
static inline float coordlen(const Coord2 v) {
	return sqrt(v.x * v.x + v.y * v.y);
}
/// Rotate vector 1 by vector 2
static inline Coord2 coordrotate(const Coord2 v1, const Coord2 v2) {
	return coord(v1.x * v2.x - v1.y * v2.y, v1.x * v2.y + v1.y * v2.x);
}
/// Normalize vector
static inline Coord2 coordnormalize(const Coord2 v) {
	return coordmult(v, 1.0f/coordlen(v));
}

typedef struct Rect{Coord2 begin, end;} Rect;

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
	int v;
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
	cpShape *shape;
	float width;
	float height;
	bool editing;
	cpVect c;
} layoutItem;
typedef map<string, layoutItem>::iterator it_layoutItems;

typedef struct textureProperties {
	string name;
	string filename;
	uint32_t gl_index;
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

struct cameraEffect {
	string n;	
	float d;
	float aStart;
	float aEnd;
	float aCurrent;
	double startTime;
};

#endif

