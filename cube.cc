#include <GL/gl.h>
#include <GL/glut.h>

#include "material.hh"
#include "drawable.hh"
#include "cube.hh"


Cube::Cube(float r, float g, float b, float x, float y, float z):
	Drawable(),
	x_(x), y_(y), z_(z)
{
    material_.setShinines(128);
    material_.setDiffuse(r, g, b);
    if (r == 1.0 || b == 1.0) g = 0.0;
    if (g == 1.0 || b == 1.0) r = 0.0;
    if (r == 1.0 || g == 1.0) b = 0.0;
    material_.setSpecular(r, g, b);
}

void Cube::draw() {
	material_.use();

	glTranslatef(x_, y_, z_);
    glutSolidCube(5);
}
