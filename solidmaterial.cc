// #include <GL/glew.h>

#include "material.hh"
#include "solidmaterial.hh"

SolidMaterial::SolidMaterial(): Material(),
	specular_(),
	diffuse_(),
	shinines_(),
	emission_()
{
}

SolidMaterial::~SolidMaterial() {
}

void SolidMaterial::setSpecular(GLfloat r, GLfloat g, GLfloat b) {
	specular_[0] = r;
	specular_[1] = g;
	specular_[2] = b;
	specular_[3] = 1.0;
}

void SolidMaterial::setDiffuse(GLfloat r, GLfloat g, GLfloat b) {
	diffuse_[0] = r;
	diffuse_[1] = g;
	diffuse_[2] = b;
	diffuse_[3] = 1.0;
}

void SolidMaterial::setEmission(GLfloat r, GLfloat g, GLfloat b) {
	emission_[0] = r;
	emission_[1] = g;
	emission_[2] = b;
	emission_[3] = 1.0;
}

void SolidMaterial::setAmbient(GLfloat r, GLfloat g, GLfloat b) {
	ambient_[0] = r;
	ambient_[1] = g;
	ambient_[2] = b;
	ambient_[3] = 1.0;
}



void SolidMaterial::setShinines(GLfloat s) {
	shinines_[0] = s;
}

void SolidMaterial::use() {
    // glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_);
    // glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_);
    // glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_);
    // glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shinines_);
    // glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission_);
}
