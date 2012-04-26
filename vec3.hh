#ifndef VEC3_HH
#define VEC3_HH

#include <cmath>

#include "common.hh"

struct Vec3 {
    Vec3(float x_, float y_, float z_):
        x(x_), y(y_), z(z_) {}

    Vec3& operator +=(const Vec3& b) {
        x += b.x;
        y += b.y;
        z += b.z;
        return *this;
    }

    Vec3& operator /=(const float b) {
        x /= b;
        y /= b;
        z /= b;
        return *this;
    }

    Vec3 operator *(const float b) {
        return Vec3(b * x, b * y, b * z);
    }

    // pistetulo
    Vec3 operator *(const Vec3& b) {
        return Vec3(y * b.z - z * b.y,
                    z * b.x - x * b.z,
                    x * b.y - y * b.x);
    }

    Vec3& operator *=(const float b) {
        x *= b;
        y *= b;
        z *= b;
        return *this;
    }

    float length() const {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }

    void normalize() {
        x /= length();
        y /= length();
        z /= length();
    }

    float x;
    float y;
    float z;
};

#endif
