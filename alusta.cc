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
    material_.setShinines(1);
    material_.setDiffuse(0.05, 0.4, 0.05);
    material_.setSpecular(0.15, 0.3, 0.15);
    // material_.setEmission(0.0, 0.2, 0.1);

}

Alusta::~Alusta()
{}

void Alusta::draw() {
	material_.use();
	// GLfloat ambMat[] = {0.2, 0.6, 0.2, 1.0};
    // glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ambMat);
    // glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ambMat);
    // glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, ambMat);
	// glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
	// glColor4fv(ambMat);

	// glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
	// GLfloat specMat[] = {0.4, 0.8, 0.4, 1.0};
	// glColor4fv(specMat);

	ObjReader::draw();
}

