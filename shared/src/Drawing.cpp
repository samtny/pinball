
#include "Drawing.h"

#ifdef __APPLE__
    #include <OpenGLES/ES1/gl.h>
    #include <OpenGLES/ES1/glext.h>
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
#ifdef __APPLE__
    glColor4f(color.r, color.g, color.b, color.a);
#else
	glColor4fv((GLfloat *)&color);
#endif
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

void DrawConstraint(cpConstraint *constraint, void *data) {
    
    
    
    cpBody *bodyA = constraint->a;
    cpBody *bodyB = constraint->b;
    
    const cpConstraintClass *klass = constraint->CP_PRIVATE(klass);
    
    if (klass == cpPinJointGetClass()) {
        cpPinJoint *joint = (cpPinJoint *)constraint;
        
        // draw
        drawSimpleJoint(bodyA, bodyB, joint->anchr1, joint->anchr2, true);
    } else if (klass == cpGrooveJointGetClass()) {
        
        cpGrooveJoint *joint = (cpGrooveJoint *)constraint;
        
        cpVect a = cpBodyLocal2World(bodyA, joint->grv_a);
        cpVect b = cpBodyLocal2World(bodyA, joint->grv_b);
        cpVect c = cpBodyLocal2World(bodyB, joint->anchr2);
        
        DrawFatSegment({a.x, a.y}, {b.x, b.y}, 0.004, LINE_COLOR, FILL_COLOR);
    }
    
    //DrawFatSegment(ap, bp, 0.01, LINE_COLOR, FILL_COLOR);
}

void drawSimpleJoint(cpBody *bodyA, cpBody *bodyB, cpVect anchr1, cpVect anchr2, bool drawLine) {
    // anchor points in world coordinates
    cpVect a = cpBodyLocal2World(bodyA, anchr1);
    cpVect b = cpBodyLocal2World(bodyB, anchr2);
    
    Coord2 ap = {a.x, a.y};
    Coord2 bp = {b.x, b.y};
    
    DrawFatSegment(ap, bp, 0.004, LINE_COLOR, FILL_COLOR);
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
				 (float)r.x, (float)r.y, 0.0f, 0.0f,
				-(float)r.y, (float)r.x, 0.0f, 0.0f,
				 (float)d.x, (float)d.y, 0.0f, 0.0f,
				 (float)a.x, (float)a.y, 0.0f, 1.0f,
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

#ifndef GL_DOUBLE
#define GL_DOUBLE 0x140A
#endif

void DrawPoints(float size, int count, Coord2 *verts, Color color)
{

	// TODO: no likey double here; need to fix Coord2 struct ASAP
	glVertexPointer(2, GL_DOUBLE, 0, verts);

	glPointSize(4);

	glColor_from_color(color);
	
    glDrawArrays(GL_POINTS, 0, count);

}

