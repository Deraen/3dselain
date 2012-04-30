#ifndef SOLIDMATERIAL_HH
#define SOLIDMATERIAL_HH

#include <GL/gl.h>

#include "material.hh"

class SolidMaterial : public Material {
public:
	SolidMaterial();

	~SolidMaterial();

	void setSpecular(GLfloat r, GLfloat g, GLfloat b);

	void setDiffuse(GLfloat r, GLfloat g, GLfloat b);

	void setEmission(GLfloat r, GLfloat g, GLfloat b);

	void setShinines(GLfloat s);

	void use();

private:
	GLfloat specular_[4];
	GLfloat diffuse_[4];
	GLfloat shinines_[4];
	GLfloat emission_[4];

};

#endif
