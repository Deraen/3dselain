#ifndef CUBE_HH
#define CUBE_HH

#include <GL/gl.h>
#include <GL/glut.h>

#include "solidmaterial.hh"
#include "drawable.hh"
#include "objreader.hh"
#include "animated.hh"

class Cube : public Drawable, public Animated {
public:
	Cube(float r, float g, float b, float x, float y, float z);

	void draw();
	bool collision(const Vec3& point, Vec3& movement);
	bool rayCollision(const Vec3& point, const Vec3& ray, float& distance);
	void animate(float time);
	void useTheForce(float time);

private:
	SolidMaterial material_;
	Vec3 pos_;
	Vec3 startpos_;
	float forceTime_;
	static ObjReader* file;
};

#endif
