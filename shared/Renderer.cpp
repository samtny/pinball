#include "Renderer.h"

#include "Physics.h"

#include "glfont2.h"

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
	_glfont->Destroy();
	delete _glfont;
}

void Renderer::setBridgeInterface(PinballBridgeInterface *bridgeInterface) {
	_bridgeInterface = bridgeInterface;
}

void Renderer::setPhysics(Physics *physics) {
	_physics = physics;
}

// TODO: this may exist, but other non-font textures will be dynamically assigned gl ids at script load time...
static int txIdFont = 101;

void Renderer::init(void) {

	_displayProperties = _bridgeInterface->getDisplayProperties();

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	// smoothing?
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POINT_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	//glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
	_glfont = new glfont::GLFont();
	_glfont->Create(_bridgeInterface->getPathForTextureFileName("font.glf"), txIdFont);
	

}

void Renderer::draw(void) {

	glClearColor(0.9, 0.9, 0.80, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);

	glViewport(_displayProperties->viewportX, _displayProperties->viewportY, _displayProperties->viewportWidth, _displayProperties->viewportHeight);

	double scale = _displayProperties->viewportWidth / _physics->getBoxWidth();

	double hw = _displayProperties->viewportWidth / scale;
	double hh = _displayProperties->viewportHeight / (_displayProperties->viewportWidth / hw);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

#ifdef __APPLE__
    glOrthof(0, hw, 0, hh, -1.0, 1.0);
    //glTranslatef(0.5, 0.5, 0.0);
#else
	glOrtho(0, hw, 0, hh, -1.0, 1.0);
    //glTranslated(0.45, 0.5, 0.0);
#endif
    
	glEnableClientState(GL_VERTEX_ARRAY);

	ChipmunkDebugDrawShapes(_physics->getSpace());	

	glDisableClientState(GL_VERTEX_ARRAY);

	// textures;
	
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 800, 0, 600, -1, 1);

	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glPushMatrix();

	glColor3f(0.0f, 0.0f, 1.0f);
	_glfont->Begin();
	_glfont->DrawString("hello world", 100, 100);

	//glTranslated(0.0, 200.0, -1.0);
	//glRotatef(15, 0, 0, 1);

	//glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

}

