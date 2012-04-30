#include <iostream>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>

#include "alusta.hh"
#include "material.hh"
#include "objreader.hh"

Alusta::Alusta():
	ObjReader("alusta2.obj"),
	material_(),
	texture_("stone.tga")
{
    material_.setShinines(100);
    material_.setDiffuse(0.75, 0.75, 0.75);
    material_.setSpecular(0.85, 0.85, 0.85);
    // material_.setEmission(0.0, 0.2, 0.1);
}

Alusta::~Alusta()
{}

void Alusta::draw() {
	material_.use();
	texture_.start();

	ObjReader::draw();

	texture_.end();
}
