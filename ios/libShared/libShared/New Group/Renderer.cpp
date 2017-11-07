#include "Renderer.h"

#include "PinballBridgeInterface.h"

#include "Playfield.h"

#include "Physics.h"

#include "Camera.h"

#include "Editor.h"

#include "Parts.h"

#include "Drawing.h"

extern "C" {
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}

#ifdef __APPLE__
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#elif __linux__
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GL/GL.h>
#include <GL/GLU.h>
#endif

#include "chipmunk/chipmunk.h"

//#include "chipmunk/ChipmunkDebugDraw.h"

#include "glfont2.h"

#include <string>
#include <map>

using std::string;
using std::map;
using std::pair;

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

void Renderer::setPlayfield(Playfield *playfield) {
	_playfield = playfield;
}

void Renderer::setPhysics(Physics *physics) {
	_physics = physics;
}

void Renderer::setEditor(Editor *editor) {
	_editor = editor;
}

// TODO: this may exist, but other non-font textures will be dynamically assigned gl ids at script load time...
static int txIdFont = 101;

void Renderer::init(void) {

	_displayProperties = _bridgeInterface->getHostProperties();

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

	this->loadFonts();

	for (it_Texture iterator = _playfield->getTextures()->begin(); iterator != _playfield->getTextures()->end(); iterator++) {

		string name = (&*iterator)->first;
		Texture *props = &(&*iterator)->second;

		glGenTextures(1, &props->gl_index);
		glBindTexture(GL_TEXTURE_2D, props->gl_index);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// TODO: change to "inject"...
		GLTexture *tex = _bridgeInterface->createRGBATexture((void *)props->filename.c_str());
		
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

	const LayoutItem box = _playfield->getLayout()->find("box")->second;
	
	// TODO: move to setter for "_displayProperties" instance??
	_scale = _displayProperties->viewportWidth / box.width;

}

void Renderer::loadFonts(void) {
	_glfont = new glfont::GLFont();
	_glfont->Create(_bridgeInterface->getTexturePath((const char *)"font.glf"), txIdFont);
}

void Renderer::draw(void) {

	glClearColor(0.0156862745098039f, 0.207843137254902f, 0.4235294117647059f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);

	this->drawPlayfield();
	this->drawOverlays();
	
	int err = glGetError();
	if (err != GL_NO_ERROR) {
		fprintf(stderr, "%x", err);
	}

}
/*
static void _drawAnchors(cpBody *body, void *data) {
	if (body->data) {
		LayoutItem *item = (LayoutItem *)body->data;
		if (item->editing == true) {
			renderer_CurrentInstance->drawAnchors(item);
		}
	}
}
*/

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
	//glTranslatef(0.375, 0.375, 0.0);

	const EditorState *s = _editor->getState();
	if (s->editMode == EDIT_MODE_PAN) {
		Coord2 diff = coordsub(s->selectionStart, s->selectionEnd);
		glTranslatef(-(float)diff.x, (float)diff.y, 0);
	}
	
	_camera->applyTransform();
	
	// TODO: separate "drawBackgrounds" method plzz...
	glEnable(GL_TEXTURE_2D);
	
	//cpSpaceEachBody(_physics->getSpace(), _drawObject, (void *)true);
	/*
	for (it_LayoutItem it = _playfield->getLayout()->begin(); it != _playfield->getLayout()->end(); it++) {

		LayoutItem item = it->second;

		drawObject(&item);

	}
	*/
	
	it_LayoutItem it = _playfield->getLayout()->find("box");
	if (it != _playfield->getLayout()->end()) {
		LayoutItem *item = &it->second;
		if (item->o->t.t != NULL) {
			drawObject(item);
		}
	}
	glDisable(GL_TEXTURE_2D);
	
	//ChipmunkDebugDrawShapes(_physics->getSpace());
	
	cpSpaceEachShape(_physics->getSpace(), DrawShape, NULL);
	
	if (s->editMode != EDIT_MODE_NONE) {
		
		// new / inserted 
		if (s->editMode == EDIT_MODE_INSERT) {
			const EditObject *o = _editor->getCurrentEditObject();

			if (o->vCurrent > 0) {
				Coord2 verts[200];
				for (int i = 0; i < o->vCurrent; i++) {
					verts[i].x = o->verts[i].x;
					verts[i].y = o->verts[i].y;
				}
				DrawPoints(20, o->vCurrent, verts, EDIT_COLOR);
			}

		}

		// alter existing
		float tx = 0;
		float ty = 0;
		float rot = 0;
		if (s->editMode == EDIT_MODE_MOVE) {
			Coord2 start = _camera->transform(s->selectionStart);
			Coord2 end = _camera->transform(s->selectionEnd);
			tx = (float)(end.x - start.x);
			ty = (float)(end.y - start.y);
		} else if (s->editMode == EDIT_MODE_ROTATE) {
			//Coord2 start = s->selectionStart;
			//Coord2 end = s->selectionEnd;
		}
		map<string, LayoutItem> *items = _playfield->getLayout();
		for (it_LayoutItem it = items->begin(); it != items->end(); it++) {

			LayoutItem item = it->second;

			if (item.editing == true) {
				
				glPushMatrix();
				
				if (s->editMode == EDIT_MODE_ROTATE) {
					
					// find center
					Coord2 c = { 0, 0 };
					for (int i = 0; i < item.o->count; i++) {
						Coord2 t = { item.v[i].x, item.v[i].y };
						c = coordadd(c, t);
					}
					c = coordmult(c, 1 / (float)item.o->count);

					// translate to origin
					glTranslatef((float)c.x, (float)c.y, 0);

					// mouse position
					Coord2 m = _camera->transform(s->selectionEnd);

					// rotvec
					Coord2 rotvec = coordsub(m, c);

					// rot
					rot = atan2f((float)rotvec.y, (float)rotvec.x) * (180.0f / (float)M_PI);

					// rotate
					glRotatef(rot, 0, 0, 1);

					// translate back
					glTranslatef((float)-c.x, (float)-c.y, 0);

				}

				glTranslatef(tx, ty, 0);

				DrawPoints(20, item.o->count, &item.v.front() , EDIT_COLOR);
				
				glPopMatrix();

			}

		}
		
	}
	
	//ChipmunkDebugDrawConstraints(_physics->getSpace());
	
	glEnable(GL_TEXTURE_2D);
	for (it_LayoutItem it = _playfield->getLayout()->begin(); it != _playfield->getLayout()->end(); it++) {
		
		LayoutItem *item = &(&*it)->second;
		
		//LayoutItem item = it->second;
		
		if (strcmp(item->n.c_str(), "box") != 0) {

			if (item->o->t.t != NULL) {
				drawObject(item);
			}
		
		}
		

	}
	glDisable(GL_TEXTURE_2D);
	
}

void Renderer::drawObject(LayoutItem *item) {

	if (strcmp(item->o->s.c_str(), "box") == 0) {
		this->drawBox(item);
	} else if (strcmp(item->o->s.c_str(), "ball") == 0) {
		this->drawBall(item);
	} else if (strcmp(item->o->s.c_str(), "flipper") == 0) {
		this->drawTexture(item);
	}
	
}

void Renderer::drawBox(LayoutItem *item) {

	static const GLfloat verts[] = {
		-0.5, -0.5,
		-0.5, 0.5,
		0.5, -0.5,
		0.5, 0.5
	};

	static const GLfloat tex[] = {
		0, 1,
		0, 0,
		1, 1,
		1, 0
	};

	glVertexPointer(2, GL_FLOAT, 0, verts);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);

	cpBody *ball = item->bodies[0];
	Texture *t = item->o->t.t;
	
	// TODO: this is just wrong; instead, precompute "center" from these coords, offset by body position above...
	float posX = (float)(item->v[3].x- item->v[0].x) / 4.0f;
	float posY = (float)(item->v[1].y - item->v[0].y) / 2.0f;

	glPushMatrix();
	//_camera->applyTransform();
	glTranslatef(posX, posY, 0);
	glScalef((float)(item->v[1].y - item->v[0].y), (float)(item->v[1].y - item->v[0].y), 0);
	glRotatef((float)ball->a * 57.2957795f, 0, 0, 1);
	
	//glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, t->gl_index);
	
	glColor4f(1, 1, 1, 1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	//glDisable(GL_TEXTURE_2D);
	
	glPopMatrix();

}

void Renderer::drawBall(LayoutItem *item) {

	static const GLfloat verts[] = {
		-0.5, -0.5,
		-0.5, 0.5,
		0.5, -0.5,
		0.5, 0.5
	};

	static const GLfloat tex[] = {
		0, 1,
		0, 0,
		1, 1,
		1, 0
	};

	glVertexPointer(2, GL_FLOAT, 0, verts);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);

	cpBody *ball = item->bodies[0];
	Texture *t = item->o->t.t;

	float posX = (float)ball->p.x;
	float posY = (float)ball->p.y;

	glPushMatrix();
	//_camera->applyTransform();
	glTranslatef(posX, posY, 0);
	glScalef(item->o->r1 * item->s * 2, item->o->r1 * item->s * 2, 0);
	glRotatef((float)ball->a * 57.2957795f, 0, 0, 1);
	
	//glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, t->gl_index);
	
	glColor4f(1, 1, 1, 1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	//glDisable(GL_TEXTURE_2D);
	
	glPopMatrix();

}

void Renderer::drawTexture(const LayoutItem *item) {
	return;
	static const GLfloat verts[] = {
		-0.5, -0.5,
		-0.5, 0.5,
		0.5, -0.5,
		0.5, 0.5
	};

	static const GLfloat tex[] = {
		0, 1,
		0, 0,
		1, 1,
		1, 0
	};

	glVertexPointer(2, GL_FLOAT, 0, verts);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);

	cpBody *body = item->bodies[0];
	
	float posX = (float)body->p.x + item->o->t.x;
	float posY = (float)body->p.y + item->o->t.y;

	glPushMatrix();

	glTranslatef(posX, posY, 0);

	//glScalef(item->o->r1 * item->s * 2, item->o->r1 * item->s * 2, 0);

	Coord2 tx = _camera->scale(coord(item->o->t.t->w * item->s, item->o->t.t->w * item->s));

	glScalef(tx.x, tx.y, 0);
	
	glRotatef((float)body->a * 57.2957795f, 0, 0, 1);
	
	glBindTexture(GL_TEXTURE_2D, item->o->t.t->gl_index);
	
	glColor4f(1, 1, 1, 1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glPopMatrix();

}

void Renderer::setOverlayText(const char *overlayName, const char *text) {

	Overlay *props = &_playfield->getOverlays()->find(overlayName)->second;
	props->v = text;

}

void Renderer::doCameraEffect(const char *effectName) {

	_camera->doEffect(effectName);

}

void Renderer::drawOverlays() {

	// font;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#ifdef __APPLE__
    // todo; get these from the correct place...
    glOrthof(0, _displayProperties->viewportWidth, 0, _displayProperties->viewportHeight, 0, 1.0);
#elif __linux__
	glOrtho(_displayProperties->viewportWidth, 0, _displayProperties->viewportHeight, 0, 0, 1);
#else
	glOrtho(0, _displayProperties->viewportWidth, 0, _displayProperties->viewportHeight, 0, 1);
#endif
   
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);

	for (it_Overlay it = _playfield->getOverlays()->begin(); it != _playfield->getOverlays()->end(); it++) {

		Overlay *props = &(&*it)->second;
		
		if (strcmp(props->t.c_str(), "text") == 0) {
			
			_glfont->Begin();
			pair<int, int> texSize;
			_glfont->GetStringSize((props->v).c_str(), &texSize);

			float txX = (float)(_displayProperties->viewportWidth * props->p.x - texSize.first / 2.0 * _displayProperties->fontScale);
			float txY = (float)(_displayProperties->viewportHeight * props->p.y + texSize.second / 2.0 * _displayProperties->fontScale);

			glPushMatrix();
			glTranslatef(txX, txY, 0);
			glScalef(_displayProperties->fontScale, _displayProperties->fontScale, 1);
			
			#ifdef __APPLE__
				glColor4f(0.839215686f, 0.701960784f, 0.376470588f, 1.0f);
			#else
				glColor3f(0.839215686f, 0.701960784f, 0.376470588f);
			#endif
			
			_glfont->DrawString((props->l + props->v).c_str(), 0, 0);
			glPopMatrix();

		} else if (strcmp(props->t.c_str(), "image") == 0) {

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

			glPushMatrix();

			float txX = 0;
			float txY = 0;
			
			const char *align = props->a.c_str();
			
			if (strcmp(align, "bl") == 0) {
				txX = (float)(props->p.x * _displayProperties->viewportWidth + props->x->w * props->s * _displayProperties->overlayScale * 0.5f);
				txY = (float)(props->p.y * _displayProperties->viewportHeight + props->x->h * props->s * _displayProperties->overlayScale * 0.5f);
			} else if (strcmp(align, "c") == 0) {
				txX = (float)(props->p.x * _displayProperties->viewportWidth);
				txY = (float)(props->p.y * _displayProperties->viewportHeight);
			} else if (strcmp(align, "r") == 0) {
				txX = (float)(props->p.x * _displayProperties->viewportWidth - props->x->w * props->s * _displayProperties->overlayScale * 0.5f);
				txY = (float)(props->p.y * _displayProperties->viewportHeight);
			} else if (strcmp(align, "tr") == 0) {
				txX = (float)(props->p.x * _displayProperties->viewportWidth - props->x->w * props->s * _displayProperties->overlayScale * 0.5f);
				txY = (float)(props->p.y * _displayProperties->viewportHeight - props->x->h * props->s * _displayProperties->overlayScale * 0.5f);
			}

			glTranslatef(txX, txY, 0);

			glScalef(props->x->w * props->s * _displayProperties->overlayScale, props->x->h * props->s * _displayProperties->overlayScale, 1);
			
			glBindTexture(GL_TEXTURE_2D, props->x->gl_index);

			#ifdef __APPLE__
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			#else
				glColor3f(1, 1, 1);
			#endif
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			glPopMatrix();

		}

	}

	glDisable(GL_TEXTURE_2D);
    
}

void Renderer::setCamera(Camera *camera) {
	_camera = camera;
}

void Renderer::setCameraMode(const char *modeName) {

	_camera->setMode(modeName);

}

void Renderer::setZoomLevel(float zoomLevel) {
	_camera->setZoomLevel(zoomLevel);
}

float Renderer::getZoomLevel() {
	return _camera->getZoomLevel();
}

