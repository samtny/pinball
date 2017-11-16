//*******************************************************************
//glfont2.cpp -- glFont Version 2.0 implementation
//Copyright (c) 1998-2002 Brad Fish
//See glfont.html for terms of use
//May 14, 2002
//*******************************************************************

//STL headers
#include <string>
#include <utility>
#include <iostream>
#include <fstream>
using namespace std;

//glFont header
#include "glfont2.h"
using namespace glfont;

//OpenGL headers
#ifdef _WIN32
#include "windows.h"
#include <GL/gl.h>
#endif

#ifdef __APPLE__
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#endif

#ifdef __linux__
#include <GL/gl.h>
#endif

//*******************************************************************
//GLFont Class Implementation
//*******************************************************************
GLFont::GLFont ()
{
	//Initialize header to safe state
	header.tex = -1;
	header.tex_width = 0;
	header.tex_height = 0;
	header.start_char = 0;
	header.end_char = 0;
	header.chars = NULL;
}
//*******************************************************************
GLFont::~GLFont ()
{
	//Destroy the font
	Destroy();
}
//*******************************************************************
bool GLFont::Create (const char *file_name, int tex)
{
	ifstream input;
	int num_chars, num_tex_bytes;
	char *tex_bytes;

	//Destroy the old font if there was one, just to be safe
	Destroy();

	//Open input file
	input.open(file_name, ios::in | ios::binary);
	if (!input)
		return false;

	//Read the header from file
	input.read((char *)&header, sizeof(header));
	header.tex = tex;

	//Allocate space for character array
	num_chars = header.end_char - header.start_char + 1;
	if ((header.chars = new GLFontChar[num_chars]) == NULL)
		return false;

	//Read character array
	input.read((char *)header.chars, sizeof(GLFontChar) *
		num_chars);

	//Read texture pixel data
	num_tex_bytes = header.tex_width * header.tex_height * 2;
	tex_bytes = new char[num_tex_bytes];
	input.read(tex_bytes, num_tex_bytes);

	//Create OpenGL texture
	glBindTexture(GL_TEXTURE_2D, tex);  
#ifdef __APPLE__
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
#endif
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
#ifdef __APPLE__
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, header.tex_width, header.tex_height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, (void *)tex_bytes);
#else
	glTexImage2D(GL_TEXTURE_2D, 0, 2, header.tex_width,
		header.tex_height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE,
		(void *)tex_bytes);
#endif
	//Free texture pixels memory
	delete[] tex_bytes;

	//Close input file
	input.close();

	//Return successfully
	return true;
}
//*******************************************************************
bool GLFont::Create (const std::string &file_name, int tex)
{
	return Create(file_name.c_str(), tex);
}
//*******************************************************************
void GLFont::Destroy (void)
{
	//Delete the character array if necessary
	if (header.chars)
	{
		delete[] header.chars;
		header.chars = NULL;
	}
}
//*******************************************************************
void GLFont::GetTexSize (std::pair<int, int> *size)
{
	//Retrieve texture size
	size->first = header.tex_width;
	size->second = header.tex_height;
}
//*******************************************************************
int GLFont::GetTexWidth (void)
{
	//Return texture width
	return header.tex_width;
}
//*******************************************************************
int GLFont::GetTexHeight (void)
{
	//Return texture height
	return header.tex_height;
}
//*******************************************************************
void GLFont::GetCharInterval (std::pair<int, int> *interval)
{
	//Retrieve character interval
	interval->first = header.start_char;
	interval->second = header.end_char;
}
//*******************************************************************
int GLFont::GetStartChar (void)
{
	//Return start character
	return header.start_char;
}
//*******************************************************************
int GLFont::GetEndChar (void)
{
	//Return end character
	return header.end_char;
}
//*******************************************************************
void GLFont::GetCharSize (int c, std::pair<int, int> *size)
{
	//Make sure character is in range
	if (c < header.start_char || c > header.end_char)
	{
		//Not a valid character, so it obviously has no size
		size->first = 0;
		size->second = 0;
	}
	else
	{
		GLFontChar *glfont_char;

		//Retrieve character size
		glfont_char = &header.chars[c - header.start_char];
		size->first = (int)(glfont_char->dx * header.tex_width);
		size->second = (int)(glfont_char->dy *
			header.tex_height);
	}
}
//*******************************************************************
/*
int GLFont::GetCharWidth (int c)
{
	//Make sure in range
	if (c < header.start_char || c > header.end_char)
		return 0;
	else
	{
		GLFontChar *glfont_char;
		
		//Retrieve character width
		glfont_char = &header.chars[c - header.start_char];
		return (int)(glfont_char->dx * header.tex_width);
	}
}
*/
//*******************************************************************
int GLFont::GetCharHeight (int c)
{
	//Make sure in range
	if (c < header.start_char || c > header.end_char)
		return 0;
	else
	{
		GLFontChar *glfont_char;

		//Retrieve character height
		glfont_char = &header.chars[c - header.start_char];
		return (int)(glfont_char->dy * header.tex_height);
	}
}
//*******************************************************************
void GLFont::Begin (void)
{
	//Bind to font texture
	glBindTexture(GL_TEXTURE_2D, header.tex);
}
//*******************************************************************

void GLFont::DrawString(const char *text, float x, float y, float scale) {
    
    this->DrawString(text, x, y);
    glScalef(scale, scale, 1);
    
}

void GLFont::DrawString(const char *text, float x, float y) {
    
		const char *i;
		GLFontChar *glfont_char;
		float width, height;
		
		static float t[12];
		static float v[18];
		
		for (i = text; *i != (const char)'\0'; i++) {
            
			if (*i < header.start_char || *i > header.end_char)
				continue;
            
			//Get pointer to glFont character
			glfont_char = &header.chars[*i - header.start_char];
            
			//Get width and height
			width = glfont_char->dx * header.tex_width;
			height = glfont_char->dy * header.tex_height;
			
			//Specify vertices and texture coordinates
            
			t[0] = glfont_char->tx1;
			t[1] = glfont_char->ty1;
			t[2] = glfont_char->tx1;
			t[3] = glfont_char->ty2;
			t[4] = glfont_char->tx2;
			t[5] = glfont_char->ty2;
            
			t[6] = glfont_char->tx2;
			t[7] = glfont_char->ty2;
			t[8] = glfont_char->tx2;
			t[9] = glfont_char->ty1;
			t[10] = glfont_char->tx1;
			t[11] = glfont_char->ty1;
			
			glTexCoordPointer(2, GL_FLOAT, 0, t);
			
			v[0] = x;
			v[1] = y;
			v[2] = 0;
			v[3] = x;
			v[4] = y - height;
			v[5] = 0;
			v[6] = x + width;
			v[7] = y - height;
			v[8] = 0;
            
			v[9] = x + width;
			v[10] = y - height;
			v[11] = 0;
			v[12] = x + width;
			v[13] = y;
			v[14] = 0;
			v[15] = x;
			v[16] = y;
			v[17] = 0;
			
			glVertexPointer(3, GL_FLOAT, 0, v);
            
			glDrawArrays(GL_TRIANGLES, 0, 6);
            
			//Move to next character
			x += width;
            
		}
		
}

//End of file

