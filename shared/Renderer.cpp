#include "Renderer.h"

#include "Physics.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "chipmunk/chipmunk.h"

#include "chipmunk/ChipmunkDebugDraw.h"

#include "glfont2.h"

#ifdef __APPLE__
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#endif

typedef struct textureProperties {
	string name;
	string filename;
	GLuint gl_index;
} textureProperties;
map<string, textureProperties> textures;
typedef map<string, textureProperties>::iterator it_textureProperties;

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
		
		// TODO: move bpp, w, h, data up to textures ivar;
		glTexImage2D(GL_TEXTURE_2D, 0, tex->bpp, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)tex->data);

		// TODO: delete textures->data instead;
		delete tex;

	}

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

	glClearColor(0.9, 0.9, 0.80, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);

	glViewport(_displayProperties->viewportX, _displayProperties->viewportY, _displayProperties->viewportWidth, _displayProperties->viewportHeight);

	this->drawPhysicsLayoutItems();
	this->drawFonts();
	
}

void Renderer::drawPhysicsLayoutItems() {
	
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
    
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	ChipmunkDebugDrawShapes(_physics->getSpace());	
	
	glEnable(GL_TEXTURE_2D);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
#ifdef __APPLE__
    glOrthof(0, hw, 0, hh, -1.0, 1.0);
    //glTranslatef(0.5, 0.5, 0.0);
#else
	glOrtho(0, hw, 0, hh, -1.0, 1.0);
    //glTranslated(0.45, 0.5, 0.0);
#endif

	for (it_layoutItems iterator = _physics->layoutItems.begin(); iterator != _physics->layoutItems.end(); iterator++) {



	}

	for (it_textureProperties iterator = textures.begin(); iterator != textures.end(); iterator++) {

		string name = iterator->first;
		textureProperties props = iterator->second;
		
		static const GLfloat vertices[] = {
			-1.0,  1.0, -0.0,
			 1.0,  1.0, -0.0,
			-1.0, -1.0, -0.0,
			 1.0, -1.0, -0.0
		};
		static const GLfloat normals[] = {
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0
		};
		static const GLfloat texCoords[] = {
			0.0, 1.0,
			1.0, 1.0,
			0.0, 0.0,
			1.0, 0.0
		};
		
		glPushMatrix();

		glLoadIdentity();

		glTranslatef(0, 0, 0.0);
		glTranslatef(_physics->_balls[0]->p.x, _physics->_balls[0]->p.y, 0);
		//glRotatef(rot, 1.0, 1.0, 1.0);
		glRotatef(_physics->_balls[0]->a * 57.2957795f, 0, 0, 1);
		glScalef(0.1, 0.1, 1);
		
		glBindTexture(GL_TEXTURE_2D, props.gl_index);
		glVertexPointer(3, GL_FLOAT, 0, vertices);
		//glNormalPointer(GL_FLOAT, 0, normals);
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);

		glColor4f(1.0, 1.0, 1.0, 1.0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		glPopMatrix();

	}
	
}

void Renderer::drawFonts() {

	// font;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
#ifdef __APPLE__
    // todo; get these from the correct place...
    glOrthof(0, 640, 0, 1136, -1.0, 1.0);
#else
	glOrtho(0, _displayProperties->viewportWidth, 0, _displayProperties->viewportHeight, -1, 1);
#endif
   
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

#ifdef __APPLE__
    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
#else
	glColor3f(0.0f, 0.0f, 1.0f);
#endif
	_glfont->Begin();
	_glfont->DrawString("abcdefghijklmnopqrstuvwxyz", 50, 75);

	glDisable(GL_TEXTURE_2D);
	
}

