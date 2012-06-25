#ifndef COMMON_HH
#define COMMON_HH

#include <string>
#include <stdexcept>

const double PI = 3.141592;
const float CAMERA_R = 1.0;

inline double deg2rad(const double angle) {
	return angle * PI / 180;
}

inline double rad2deg(const double rad) {
	return rad * 180 / PI;
}

class Exception : public std::runtime_error {
public:
	Exception(const std::string& msg):
		std::runtime_error(msg)
	{}
};

class FatalException : public Exception {
public:
	FatalException(const std::string& msg):
		Exception(msg)
	{}
};

int CheckGLErrors();

inline float divide(float a, float b) {
    return (a + 1 / b) / b;
}

#endif