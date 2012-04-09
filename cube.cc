#include <GL/gl.h>
#include <GL/glut.h>

#include "material.hh"
#include "drawable.hh"
#include "cube.hh"


Cube::Cube(Material* material, float x, float y, float z):
	Drawable(),
	material_(material),
	x_(x), y_(y), z_(z)
{}

void Cube::draw() {
	material_->use();

	glTranslatef(x_, y_, z_);
    glutSolidCube(5);
}
