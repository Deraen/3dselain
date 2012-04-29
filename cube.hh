#ifndef CUBE_HH
#define CUBE_HH

#include <GL/gl.h>
#include <GL/glut.h>

#include "solidmaterial.hh"
#include "drawable.hh"
#include "objreader.hh"

class Cube : public Drawable {
public:
	Cube(float r, float g, float b, float x, float y, float z);

	void draw();
	bool collision(const Vec3& point, Vec3& movement, unsigned int depth = 0);

private:
	SolidMaterial material_;
	Vec3 pos_;
	static ObjReader* file;
};

#endif
