#include "Renderer.h"

#include "Physics.h"

#include "Camera.h"

#include "Parts.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#ifdef __APPLE__
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#else
#include <GL/GL.h>
#include <GL/GLU.h>
#endif

#include "chipmunk/chipmunk.h"

#include "chipmunk/ChipmunkDebugDraw.h"

#include "glfont2.h"

#include <string>
#include <map>

using namespace std;

typedef struct textureProperties {
	string name;
	string filename;
	GLuint gl_index;
	int w;
	int h;
} textureProperties;
map<string, textureProperties> textures;
typedef map<string, textureProperties>::iterator it_textureProperties;

static Renderer *renderer_CurrentInstance;

Renderer::Renderer(void)
{
	renderer_CurrentInstance = this;
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
    
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	this->loadTextures();
	this->loadFonts();

	for (it_textureProperties iterator = textures.begin(); iterator != textures.end(); iterator++) {

		string name = (&*iterator)->first;
		textureProperties *props = &(&*iterator)->second;

		glGenTextures(1, &props->gl_index);
		glBindTexture(GL_TEXTURE_2D, props->gl_index);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// TODO: change to "inject"...
		Texture *tex = _bridgeInterface->createRGBATexture((void *)props->filename.c_str());
		
#ifdef __APPLE__
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)tex->data);
#else
        glTexImage2D(GL_TEXTURE_2D, 0, 4, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)tex->data);
#endif
		props->w = tex->width;
		props->h = tex->height;

		// TODO: delete textures->data instead;
		delete tex;

	}

	const layoutItem *box = &_physics->getLayoutItems()->find("box")->second;
	
	// TODO: move to setter for "_displayProperties" instance??
	_scale = _displayProperties->viewportWidth / box->width;

	_camera = new Camera();
	_camera->setWorldScale(_scale);
	_camera->setDisplayProperties(_displayProperties);

	_camera->setPhysics(_physics);

	this->setCameraFollowsBall();

}

void Renderer::loadTextures(void) {

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	const char *texturesFileName = _bridgeInterface->getPathForScriptFileName((void *)"textures.lua");

	int error = luaL_dofile(L, texturesFileName);
	if (!error) {

        lua_getglobal(L, "textures");

		if (lua_istable(L, -1)) {
			
			lua_pushnil(L);
			while(lua_next(L, -2) != 0) {
				
				const char *name = lua_tostring(L, -2);

				textureProperties props = { "", "", -1 };
				props.name = name;
				
				lua_pushnil(L);
				while (lua_next(L, -2) != 0) {

					const char *key = lua_tostring(L, -2);
                    
					if (strcmp("filename", key) == 0) {
						
						props.filename = lua_tostring(L, -1);
                        
					}
                    
					lua_pop(L, 1);
				}

				textures.insert(make_pair(name, props));

				lua_pop(L, 1);

			}
            
		}
        
		lua_pop(L, 1); // pop table

    } else {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  // pop err from lua stack
	}

	lua_close(L);

}

void Renderer::loadFonts(void) {
	_glfont = new glfont::GLFont();
	_glfont->Create(_bridgeInterface->getPathForTextureFileName((void *)"font.glf"), txIdFont);
}

void Renderer::draw(void) {

	glClearColor(0.9f, 0.9f, 0.80f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);

	this->drawPlayfield();
	this->drawFonts();
	
	int err = glGetError();
	if (err != GL_NO_ERROR) {
		fprintf(stderr, "%x", err);
	}

}

static void _drawObject(cpBody *body, void *data) {
	renderer_CurrentInstance->drawObject(body, data);
}

void Renderer::drawPlayfield() {
	
	glViewport(0, 0, _displayProperties->viewportWidth, _displayProperties->viewportHeight);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#ifdef __APPLE__
    glOrthof(0, _displayProperties->viewportWidth, 0, _displayProperties->viewportHeight, 0, 1);
#else
	gluOrtho2D(0, _displayProperties->viewportWidth, 0, _displayProperties->viewportHeight);
#endif
    
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.375, 0.375, 0.0);

	// this probably belongs to "camera"...
	const layoutItem *box = &_physics->getLayoutItems()->find("box")->second;
	_scale = _displayProperties->viewportWidth / box->width;

	glPushMatrix();
	_camera->setWorldScale(_scale);
	_camera->applyTransform();
	glScalef(_scale, _scale, 1);
	ChipmunkDebugDrawShapes(_physics->getSpace());
	glPopMatrix();

	cpSpaceEachBody(_physics->getSpace(), _drawObject, NULL);
	
}

void Renderer::drawObject(cpBody *body, void *data) {

	if (body->data) {
		layoutItem *item = (layoutItem *)body->data;
		if (strcmp(item->o.s.c_str(), "ball") == 0) {
			this->drawBall(item);
		}
	}

}

void Renderer::drawBall(layoutItem *item) {

	static const GLfloat verts[] = {
		-0.5, -0.5,
		-0.5, 0.5,
		0.5, -0.5,
		0.5, 0.5
	};

	static const GLfloat tex[] = {
		0, 0,
		0, 1,
		1, 0,
		1, 1
	};

	glVertexPointer(2, GL_FLOAT, 0, verts);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);

	cpBody *ball = item->body;
	textureProperties *t = &textures[item->o.t.n];

	// lerp
	float posX = (float)ball->p.x * _scale;
	float posY = (float)ball->p.y * _scale;

	glPushMatrix();
	_camera->applyTransform();
	glTranslatef(posX, posY, 0);
	glScalef(item->o.r1 * 2 * _scale, item->o.r1 * 2 * _scale, 0);
	glRotatef((float)ball->a * 57.2957795f, 0, 0, 1);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, t->gl_index);
	
	glColor4f(1, 1, 1, 1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glDisable(GL_TEXTURE_2D);
	
	glPopMatrix();

}

void Renderer::drawFonts() {

	// font;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#ifdef __APPLE__
    // todo; get these from the correct place...
    glOrthof(0, _displayProperties->viewportWidth, 0, _displayProperties->viewportHeight, 0, 1.0);
#else
	glOrtho(0, _displayProperties->viewportWidth, 0, _displayProperties->viewportHeight, 0, 1);
#endif
   
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

#ifdef __APPLE__
    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
#else
	glColor3f(0.0f, 0.0f, 1.0f);
#endif

	glEnable(GL_TEXTURE_2D);
	_glfont->Begin();
	pair<int, int> texSize;
	_glfont->GetStringSize("abcdefghijklmnopqrstuvwxyz", &texSize);
    
	float txX = (float)(_displayProperties->viewportWidth * 0.5 - texSize.first / 2.0 * _displayProperties->fontScale);
	float txY = (float)(_displayProperties->viewportHeight * 0.95 + texSize.second / 2.0 * _displayProperties->fontScale);

    glTranslatef(txX, txY, 0);
    glScalef(_displayProperties->fontScale, _displayProperties->fontScale, 1);
    
	_glfont->DrawString("abcdefghijklmnopqrstuvwxyz", 0, 0);
    
	glDisable(GL_TEXTURE_2D);
    
}

void Renderer::setCameraFollowsBall(void) {

	_camera->setModeFollowBall();

}

void Renderer::setZoomLevel(float zoomLevel) {
	_camera->setZoomLevel(zoomLevel);
}

float Renderer::getZoomLevel() {
	return _camera->getZoomLevel();
}

