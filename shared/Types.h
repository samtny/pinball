
#ifndef __PINBALL_TYPES__
#define __PINBALL_TYPES__

#include <math.h>

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

typedef struct Rect{
	Coord2 begin, end;
} Rect;

#endif

