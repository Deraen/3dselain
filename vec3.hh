#include <cmath>

const double PI = 3.141592;

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

    void rotateYX(const float yAngle, const float xAngle) {
        float yRad = yAngle * PI / 180;
        float xRad = xAngle * PI / 180;
        x = x * cos(yRad) + y * sin(yRad) * sin(xRad) + z * sin(yRad) * cos(xRad);
        y = y * cos(xRad) + z * -sin(xRad);
        z = x * -sin(yRad) + y * cos(yRad) * sin(xRad) + z * cos(yRad) * cos(xRad);
    }

    void rotateY(const float angle) {
        //       | cos(a)  0 sin(a) |
        // R_y = | 0       1 0      |
        //       | -sin(a) 0 cos(a) |
        float rad = angle * PI / 180;
        x = x * cos(rad) + z * sin(rad);
        z = x * -sin(rad) + z * cos(rad);
    }

    void rotateX(const float angle) {
        //       | 1 0      0       |
        // R_x = | 0 cos(a) -sin(a) |
        //       | 0 sin(a) cos(a)  |
        float rad = angle * PI / 180;
        y = y * cos(rad) + z * -sin(rad);
        z = y * sin(rad) + z * cos(rad);
    }

    double length() const {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }

    double x;
    double y;
    double z;
};