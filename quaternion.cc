
#include "quaternion.hh"

Quaternion::Quaternion(float x, float y, float z, float w):
	x_(x), y_(y), z_(z), w_(w)
{}

Quaternion::Quaternion(const Vec3& v, float angle) {
	Vec3 vn(v);
	vn.normalize();

	float sinAngle = sin(0.5 * angle);

	x_ = vn.x * sinAngle;
	y_ = vn.y * sinAngle;
	z_ = vn.z * sinAngle;
	w_ = cos(0.5 * angle);
}

// normalising a quaternion works similar to a vector. This method will not do anything
// if the quaternion is close enough to being unit-length. define TOLERANCE as something
// small like 0.00001f to get accurate results
void Quaternion::normalize()
{
	// Don't normalize if we don't have to
	float mag2 = w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_;
	if (fabs(mag2) > TOLERANCE && fabs(mag2 - 1.0f) > TOLERANCE) { // fabs = float abs
		float mag = sqrt(mag2);
		w_ /= mag;
		x_ /= mag;
		y_ /= mag;
		z_ /= mag;
	}
}

// We need to get the inverse of a quaternion to properly apply a quaternion-rotation to a vector
// The conjugate of a quaternion is the same as the inverse, as long as the quaternion is unit-length
Quaternion Quaternion::getConjugate() const
{
	return Quaternion(-x_, -y_, -z_, w_);
}

// Multiplying q1 with q2 applies the rotation q2 to q1
Quaternion Quaternion::operator* (const Quaternion &rq) const
{
	// the constructor takes its arguments as (x, y, z, w)
	return Quaternion(w_ * rq.x_ + x_ * rq.w_ + y_ * rq.z_ - z_ * rq.y_,
	                  w_ * rq.y_ + y_ * rq.w_ + z_ * rq.x_ - x_ * rq.z_,
	                  w_ * rq.z_ + z_ * rq.w_ + x_ * rq.y_ - y_ * rq.x_,
	                  w_ * rq.w_ - x_ * rq.x_ - y_ * rq.y_ - z_ * rq.z_);
}

// Multiplying a quaternion q with a vector v applies the q-rotation to v
Vec3 Quaternion::operator* (const Vec3 &vec) const
{
	Vec3 vn(vec);
	vn.normalize();

	Quaternion vecQuat(vn.x, vn.y, vn.z, 0.0);

	Quaternion resQuat = vecQuat * getConjugate();
	resQuat = *this * resQuat;

	return Vec3(resQuat.x_, resQuat.y_, resQuat.z_);
}

float Quaternion::getHeading() const {
	// return atan2(2*(x_ * w_ + y_ * z_), 1 - 2 * (z_ * z_ + w_ * w_));
	return atan((2 * (x_ * w_ + y_ * z_)) / (1 - 2 * (z_ * z_ + w_ * w_)));
}

float Quaternion::getPitch() const {
	return asin(2*(x_ * z_ * y_ * w_));
}

// Convert to Axis/Angles
// float Quaternion::getAxisAngle(Vec3 axis)
// {
// 	float scale = sqrt(x_ * x_ + y_ * y_ + z_ * z_);
// 	axis.x = x_ / scale;
// 	axis.y = y_ / scale;
// 	axis.z = z_ / scale;
// 	return acos(w_) * 2.0;
// }
