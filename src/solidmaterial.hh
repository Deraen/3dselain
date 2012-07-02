#ifndef SOLIDMATERIAL_HH
#define SOLIDMATERIAL_HH

#include <GL3/gl3w.h>

#include "material.hh"

class SolidMaterial : public Material {
public:
	SolidMaterial();

	~SolidMaterial();

	void setSpecular(GLfloat r, GLfloat g, GLfloat b);

	void setDiffuse(GLfloat r, GLfloat g, GLfloat b);

	void setEmission(GLfloat r, GLfloat g, GLfloat b);

	void setAmbient(GLfloat r, GLfloat g, GLfloat b);

	void setShinines(GLfloat s);

	void setOpacity(float v) { opacity_ = v; }

	void use();

	float* diffuse() { return ambient_; }
	float* specular() { return specular_; }
	float* ambient() { return ambient_; }
	float opacity() { return opacity_; }

private:
	GLfloat specular_[4];
	GLfloat diffuse_[4];
	GLfloat shinines_[4];
	GLfloat emission_[4];
	GLfloat ambient_[4];
	float opacity_;
};

#endif
