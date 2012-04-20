#ifndef VEC3_HH
#define VEC3_HH

#include <cmath>

#include "common.hh"

struct Vec3 {
    Vec3(double x_, double y_, double z_):
        x(x_), y(y_), z(z_) {}

    Vec3& operator +=(const Vec3& b) {
        x += b.x;
        y += b.y;
        z += b.z;
        return *this;
    }

    Vec3& operator /=(const double b) {
        x /= b;
        y /= b;
        z /= b;
        return *this;
    }

    Vec3 operator *(const double b) {
        return Vec3(b * x, b * y, b * z);
    }

    Vec3& operator *=(const double b) {
        x *= b;
        y *= b;
        z *= b;
        return *this;
    }

    double length() const {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }

    void normalize() {
        x /= length();
        y /= length();
        z /= length();
    }

    double x;
    double y;
    double z;
};

#endif
