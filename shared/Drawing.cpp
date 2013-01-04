
#include "Drawing.h"

#ifdef __APPLE__
	#include "OpenGL/gl.h"
	#include "OpenGL/glu.h"
	#include <GLUT/glut.h>
#else
	#ifdef _WIN32
		#include <windows.h>
	#endif
	
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include "GL/glut.h"
#endif

static inline void
glColor_from_color(Color color){
	glColor4fv((GLfloat *)&color);
}

void DrawShape(cpShape *shape, void *data) {

	cpBody *body = shape->body;

	switch (shape->klass_private->type) {
	case CP_CIRCLE_SHAPE: {
		cpCircleShape *circle = (cpCircleShape *)shape;
		Coord2 tc = {circle->tc.x, circle->tc.y};
		DrawCircle(tc, body->a, circle->r, LINE_COLOR, FILL_COLOR);
		break;
		}
	case CP_SEGMENT_SHAPE: {
		cpSegmentShape *segment = (cpSegmentShape *)shape;
		Coord2 ta = {segment->ta.x, segment->ta.y};
		Coord2 tb = {segment->tb.x, segment->tb.y};
		DrawFatSegment(ta, tb, segment->r, LINE_COLOR, FILL_COLOR);
		break;
		}
	default:
		break;
	}

}

void DrawCircle(Coord2 center, float angle, float radius, Color lineColor, Color fillColor) {

	glVertexPointer(2, GL_FLOAT, 0, circleVAR);

	glPushMatrix(); {
		glTranslatef(center.x, center.y, 0.0f);
		glRotatef(angle*180.0f/M_PI, 0.0f, 0.0f, 1.0f);
		glScalef(radius, radius, 1.0f);
		
		if(fillColor.a > 0){
			glColor_from_color(fillColor);
			glDrawArrays(GL_TRIANGLE_FAN, 0, circleVAR_count - 1);
		}
		
		if(lineColor.a > 0){
			glColor_from_color(lineColor);
			glDrawArrays(GL_LINE_STRIP, 0, circleVAR_count);
		}
	} glPopMatrix();

}

void DrawFatSegment(Coord2 a, Coord2 b, float radius, Color lineColor, Color fillColor) {

	if(radius){
		glVertexPointer(3, GL_FLOAT, 0, pillVAR);
		glPushMatrix(); {
			Coord2 d = coordsub(b, a);
			Coord2 r = coordmult(d, radius/coordlen(d));

			const GLfloat matrix[] = {
				 r.x, r.y, 0.0f, 0.0f,
				-r.y, r.x, 0.0f, 0.0f,
				 d.x, d.y, 0.0f, 0.0f,
				 a.x, a.y, 0.0f, 1.0f,
			};
			glMultMatrixf(matrix);
			
			if(fillColor.a > 0){
				glColor_from_color(fillColor);
				glDrawArrays(GL_TRIANGLE_FAN, 0, pillVAR_count);
			}
			
			if(lineColor.a > 0){
				glColor_from_color(lineColor);
				glDrawArrays(GL_LINE_LOOP, 0, pillVAR_count);
			}
		} glPopMatrix();
	} else {
		//ChipmunkDebugDrawSegment(a, b, lineColor);
	}

}

void DrawPoints(float size, int count, Coord2 *verts, Color color)
{

	glVertexPointer(2, GL_FLOAT, 0, verts);

	glPointSize(4);

	glColor4f(color.r, color.g, color.b, color.a);
	
	glBegin(GL_POINTS); {
		for (int i = 0; i < count; i++) {
			glVertex2f(verts[i].x, verts[i].y);
		}
	} glEnd();

}