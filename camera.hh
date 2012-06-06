#ifndef CAMERA_HH
#define CAMERA_HH

#include <GL/glew.h>
#include "common.hh"
#include "vec3.hh"
#include "matrix.hh"

class Camera {
public:
    Camera(const float x, const float y, const float z);

    void move(float amount);

    void strafe(float amount);

    void moveHeight(float amount);
    void pitch(float amount);

    void heading(float amount);

    Vec3 getMovement() const;

    void applyMovement(const Vec3& move);

    // void set();
    float* modelview() { return rot_.data(); }
    float* projection(unsigned int w, unsigned int h);
    float* location();

    Vec3 getPos() const;
    Vec3 getVector() const;

private:
    Vec3 pos_;
    Vec3 delta_;
    Matrix rot_;
    Matrix projection_;
    Matrix location_;
};

#endif
