#ifndef CUBE_HH
#define CUBE_HH

#include <GL/gl.h>
#include <GL/glut.h>

#include "solidmaterial.hh"
#include "drawable.hh"
#include "boundingbox.hh"
#include "objreader.hh"

class Cube : public Drawable, public ObjReader {
public:
	Cube(float r, float g, float b, float x, float y, float z);

	void draw();
	bool collision(BoundingBox& box) {
		return box_.collision(box);
	}

private:
	SolidMaterial material_;
	BoundingBox box_;
};

#endif
