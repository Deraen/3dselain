#ifndef VEC3_HH
#define VEC3_HH

#include <cmath>

#include "common.hh"
#include "debug.hh"

struct Vec3 {
    Vec3(float x_, float y_, float z_):
        x(x_), y(y_), z(z_) {}

    Vec3& operator +=(const Vec3& b) {
        x += b.x;
        y += b.y;
        z += b.z;
        return *this;
    }

    Vec3& operator -=(const Vec3& b) {
        x -= b.x;
        y -= b.y;
        z -= b.z;
        return *this;
    }

    Vec3& operator /=(const float b) {
        x /= b;
        y /= b;
        z /= b;
        return *this;
    }

    Vec3 operator /(const float b) const {
        return Vec3(x / b,
                    y / b,
                    z / b);
    }

    Vec3 operator +(const Vec3& b) const {
        return Vec3(x + b.x,
                    y + b.y,
                    z + b.z);
    }

    Vec3 operator -(const Vec3& b) const {
        return Vec3(x - b.x,
                    y - b.y,
                    z - b.z);
    }

    Vec3 operator *(const float b) const {
        return Vec3(b * x, b * y, b * z);
    }

    // pistetulo
    Vec3 cross(const Vec3& b) const {
        return Vec3(y * b.z - z * b.y,
                    z * b.x - x * b.z,
                    x * b.y - y * b.x);
    }

    float dot(const Vec3& b) const {
        return x * b.x + y * b.y + z * b.z;
    }

    Vec3& operator *=(const float b) {
        x *= b;
        y *= b;
        z *= b;
        return *this;
    }

    bool inside(const Vec3& min, const Vec3& max) const {
        return (min.x <= x && x <= max.x &&
                min.y <= y && y <= max.y &&
                min.z <= z && z <= max.z);
    }

    friend Debug& operator<<(Debug& debug, const Vec3& b) {
        debug << b.x << ", " << b.y << ", " << b.z;
        return debug;
    }

    float length() const {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }

    float distanceToPlane(const Vec3& normal, const float d) {
        return normal.x * x + normal.y * y + normal.z * z + d;
    }

    Vec3& normalize() {
        x /= length();
        y /= length();
        z /= length();
        return *this;
    }

    float x;
    float y;
    float z;
};

#endif
