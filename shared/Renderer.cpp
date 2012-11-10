#include "Renderer.h"

#include "Physics.h"

#ifdef __APPLE__
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#endif

#include "chipmunk.h"

#include "ChipmunkDebugDraw.h"

Renderer::Renderer(void)
{

}

Renderer::~Renderer(void)
{

}

void Renderer::setBridgeInterface(PinballBridgeInterface *bridgeInterface) {
	_bridgeInterface = bridgeInterface;
}

void Renderer::setPhysics(Physics *physics) {
	_physics = physics;
}

void Renderer::init(void) {

	_displayProperties = _bridgeInterface->getDisplayProperties();

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void Renderer::draw(void) {

	glClearColor(0.9, 0.9, 0.80, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glViewport(_displayProperties->viewportX, _displayProperties->viewportY, _displayProperties->viewportWidth, _displayProperties->viewportHeight);
	
	//double scale = cpfmin(_displayProperties->viewportWidth/boxWidth, _displayProperties->viewportHeight/boxHeight);

	double hw = _displayProperties->viewportWidth * (0.5 / _displayProperties->scale);
	double hh = _displayProperties->viewportHeight * (0.5 / _displayProperties->scale);
	
	ChipmunkDebugDrawPointLineScale = _displayProperties->scale;
	
	glLineWidth(1);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    
	//glOrtho(0, hw, 0, hh, -1.0, 1.0);
    
    static const GLfloat multMatrix[] = {
        hw, 0, 0, -(hw + 0)/(hw - 0),
        0, hh, 0, -(hh + 0)/(hh - 0),
        0, 0, -1, 0,
        0, 0, 0, 1
    };
    
    glMultMatrixf(multMatrix);
    
	//glTranslated(0.5, 0.5, 0.0); // margin
    
    glTranslatef(0.5, 0.5, 0.0);

	ChipmunkDebugDrawShapes(_physics->getSpace());	

}

