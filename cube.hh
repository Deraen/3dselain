#ifndef CUBE_HH
#define CUBE_HH

#include <GL/gl.h>
#include <GL/glut.h>

#include "solidmaterial.hh"
#include "drawable.hh"

class Cube : public Drawable {
public:
	Cube(float r, float g, float b, float x, float y, float z);

	void draw() ;

private:
	SolidMaterial material_;
	float x_;
	float y_;
	float z_;
};

#endif
