#ifndef CUBE_HH
#define CUBE_HH

#include <GL/gl.h>
#include <GL/glut.h>

#include "material.hh"
#include "drawable.hh"

class Cube : public Drawable {
public:
	Cube(Material* material, float x, float y, float z);

	void draw() ;

private:
	Material* material_;
	float x_;
	float y_;
	float z_;
};

#endif
