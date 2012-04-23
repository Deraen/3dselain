#include <GL/gl.h>
#include <GL/glut.h>

#include "material.hh"
#include "drawable.hh"
#include "cube.hh"
#include "vec3.hh"
#include "objreader.hh"

Cube::Cube(float r, float g, float b, float x, float y, float z):
	Drawable(),
    ObjReader("cube.obj"),
    box_(Vec3(x, y, z), Vec3(5.0, 5.0, 5.0))
{
    material_.setShinines(128);
    material_.setDiffuse(r, g, b);
    // if (r == 1.0 || b == 1.0) g = 0.4;
    // if (g == 1.0 || b == 1.0) r = 0.4;
    // if (r == 1.0 || g == 1.0) b = 0.4;
    material_.setSpecular(r, g, b);

    // ObjReader lukija("cube.obj");
}

void Cube::draw() {
	material_.use();

	glTranslatef(box_.pos.x, box_.pos.y, box_.pos.z);

    ObjReader::draw();
    // glutSolidCube(5);
}

