
#ifndef __DRAWING__
#define __DRAWING__

#include "Parts.h"

typedef float GLfloat;

typedef struct Color {
	float r, g, b, a;
} Color;

const Color LINE_COLOR = {.25, .25, .25, 1};
const Color FILL_COLOR = {.75, .75, .75, 1};
const Color EDIT_COLOR = {1, 1, 1, 1};

void DrawShape(cpShape *shape, void *data);

void DrawConstraint(cpConstraint *constraint, void *data);

static const GLfloat circleVAR[] = {
	 0.0000f,  1.0000f,
	 0.2588f,  0.9659f,
	 0.5000f,  0.8660f,
	 0.7071f,  0.7071f,
	 0.8660f,  0.5000f,
	 0.9659f,  0.2588f,
	 1.0000f,  0.0000f,
	 0.9659f, -0.2588f,
	 0.8660f, -0.5000f,
	 0.7071f, -0.7071f,
	 0.5000f, -0.8660f,
	 0.2588f, -0.9659f,
	 0.0000f, -1.0000f,
	-0.2588f, -0.9659f,
	-0.5000f, -0.8660f,
	-0.7071f, -0.7071f,
	-0.8660f, -0.5000f,
	-0.9659f, -0.2588f,
	-1.0000f, -0.0000f,
	-0.9659f,  0.2588f,
	-0.8660f,  0.5000f,
	-0.7071f,  0.7071f,
	-0.5000f,  0.8660f,
	-0.2588f,  0.9659f,
	 0.0000f,  1.0000f,
	 0.0f, 0.0f, // For an extra line to see the rotation.
};
static const int circleVAR_count = sizeof(circleVAR)/sizeof(GLfloat)/2;

void DrawCircle(Coord2 center, float angle, float radius, Color lineColor, Color fillColor);

static const GLfloat pillVAR[] = {
	 0.0000f,  1.0000f, 1.0f,
	 0.2588f,  0.9659f, 1.0f,
	 0.5000f,  0.8660f, 1.0f,
	 0.7071f,  0.7071f, 1.0f,
	 0.8660f,  0.5000f, 1.0f,
	 0.9659f,  0.2588f, 1.0f,
	 1.0000f,  0.0000f, 1.0f,
	 0.9659f, -0.2588f, 1.0f,
	 0.8660f, -0.5000f, 1.0f,
	 0.7071f, -0.7071f, 1.0f,
	 0.5000f, -0.8660f, 1.0f,
	 0.2588f, -0.9659f, 1.0f,
	 0.0000f, -1.0000f, 1.0f,

	 0.0000f, -1.0000f, 0.0f,
	-0.2588f, -0.9659f, 0.0f,
	-0.5000f, -0.8660f, 0.0f,
	-0.7071f, -0.7071f, 0.0f,
	-0.8660f, -0.5000f, 0.0f,
	-0.9659f, -0.2588f, 0.0f,
	-1.0000f, -0.0000f, 0.0f,
	-0.9659f,  0.2588f, 0.0f,
	-0.8660f,  0.5000f, 0.0f,
	-0.7071f,  0.7071f, 0.0f,
	-0.5000f,  0.8660f, 0.0f,
	-0.2588f,  0.9659f, 0.0f,
	 0.0000f,  1.0000f, 0.0f,
};
static const int pillVAR_count = sizeof(pillVAR)/sizeof(GLfloat)/3;

void DrawFatSegment(Coord2 a, Coord2 b, float radius, Color lineColor, Color fillColor);

void DrawPoints(float size, int count, Coord2 *verts, Color color);

void drawSimpleJoint(cpBody *bodyA, cpBody *bodyB, cpVect anchr1, cpVect anchr2, bool drawLine);

#endif

