#ifndef QUATERNION_HH
#define QUATERNION_HH

#include <cmath>

#include "vec3.hh"

const float TOLERANCE = 0.000001;
const float PIOVER180 = PI / 180;

class Quaternion {
public:
	Quaternion(float x, float y, float z, float w);
	Quaternion(const Vec3 &vec, float angle);

	void normalize();
	Quaternion getConjugate() const;
	Quaternion operator* (const Quaternion &rq) const;
	Vec3 operator* (const Vec3 &vec) const;

	float getPitch() const;
	float getHeading() const;
	// float getAxisAngle(Vec3 axis);
private:
	float x_;
	float y_;
	float z_;
	float w_;
};

#endif
