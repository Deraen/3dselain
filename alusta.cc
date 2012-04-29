#include <iostream>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>

#include "alusta.hh"
#include "material.hh"
#include "objreader.hh"

Alusta::Alusta():
	ObjReader("alusta2.obj"),
	material_()
{
    material_.setShinines(1);
    material_.setDiffuse(0.05, 0.4, 0.05);
    material_.setSpecular(0.15, 0.3, 0.15);
    // material_.setEmission(0.0, 0.2, 0.1);

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    // select modulate to mix texture with color for shading
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	// when texture area is small, bilinear filter the closest mipmap
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	                 GL_LINEAR_MIPMAP_NEAREST );
	// when texture area is large, bilinear filter the original
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// the texture wraps over at the edges (repeat)
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	gluBuild2DMipmaps( GL_TEXTURE_2D, 3, Stone::width, Stone::height,
                   GL_RGB, GL_UNSIGNED_BYTE, Stone::header_data );
}

Alusta::~Alusta()
{}

void Alusta::draw() {
	material_.use();

	ObjReader::draw();
}
