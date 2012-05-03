#ifndef COMMON_HH
#define COMMON_HH

const double PI = 3.141592;
const float CAMERA_R = 1.0;

inline double deg2rad(const double angle) {
	return angle * PI / 180;
}

inline double rad2deg(const double rad) {
	return rad * 180 / PI;
}

#endif