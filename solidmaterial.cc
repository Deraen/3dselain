#include <GL/gl.h>

#include "material.hh"
#include "solidmaterial.hh"

SolidMaterial::SolidMaterial(): Material(),
	specular_(NULL),
	diffuse_(NULL),
	shinines_(NULL)
{}

SolidMaterial::~SolidMaterial() {
	delete[] specular_;
	delete[] diffuse_;
	delete[] shinines_;
}

void SolidMaterial::setSpecular(GLfloat r, GLfloat g, GLfloat b) {
	if (specular_ == NULL) specular_ = new GLfloat[3];
	specular_[0] = r;
	specular_[1] = g;
	specular_[2] = b;
}

void SolidMaterial::setDiffuse(GLfloat r, GLfloat g, GLfloat b) {
	if (diffuse_ == NULL) diffuse_ = new GLfloat[3];
	diffuse_[0] = r;
	diffuse_[1] = g;
	diffuse_[2] = b;
}

void SolidMaterial::setShinines(GLfloat s) {
	if (shinines_ == NULL) shinines_ = new GLfloat[1];
	shinines_[0] = s;
}

void SolidMaterial::use() {
    if (specular_ != NULL) glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_);
    if (diffuse_ != NULL)  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_);
    if (shinines_ != NULL) glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shinines_);
}
