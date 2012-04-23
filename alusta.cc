#include <iostream>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>

#include "alusta.hh"
// #include "alusta.h" //data
#include "material.hh"
#include "objreader.hh"

Alusta::Alusta():
	ObjReader("alusta.obj"),
	material_()
{
    material_.setShinines(128);
    material_.setDiffuse(0.2, 1.0, 0.2);
    material_.setSpecular(0.5, 1.0, 0.5);

}

Alusta::~Alusta()
{}

void Alusta::draw() {
	material_.use();
	// glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
	// GLfloat ambMat[] = {0.2, 0.6, 0.2, 1.0};
	// glColor4fv(ambMat);

	// glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
	// GLfloat specMat[] = {0.4, 0.8, 0.4, 1.0};
	// glColor4fv(specMat);

	ObjReader::draw();
}

