#ifndef DRAWABLE_HH
#define DRAWABLE_HH

#include "vec3.hh"

class Drawable {
public:
	Drawable() {};
	virtual void draw() = 0;
	virtual ~Drawable() {}
	virtual bool collision(const Vec3& point, Vec3& movement, unsigned int depth = 0) = 0;

private:
};

#endif
