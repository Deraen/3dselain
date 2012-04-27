#include <GL/gl.h>
#include <GL/glut.h>

#include "material.hh"
#include "drawable.hh"
#include "cube.hh"
#include "vec3.hh"
#include "objreader.hh"

ObjReader* Cube::file = NULL;

Cube::Cube(float r, float g, float b, float x, float y, float z):
    // ObjReader("cube.obj"),
    material_(),
    pos_(x, y, z)
{
    if (file == NULL) file = new ObjReader("cube.obj");

    material_.setShinines(128);
    material_.setDiffuse(r, g, b);
    // if (r == 1.0 || b == 1.0) g = 0.4;
    // if (g == 1.0 || b == 1.0) r = 0.4;
    // if (r == 1.0 || g == 1.0) b = 0.4;
    material_.setSpecular(r, g, b);
    // material_.setEmission(1.0, 0.0, 0.0);
}

void Cube::draw() {
	material_.use();

	glTranslatef(pos_.x, pos_.y, pos_.z);

    file->draw();
}

bool Cube::collision(const Vec3 &point, Vec3 &movement) {
    // XXX: collision olettaa että kappale on maailma koordinaateissa
    return file->collision(point, movement);
}
