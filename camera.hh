#ifndef CAMERA_HH
#define CAMERA_HH

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>
#include "common.hh"
#include "vec3.hh"
#include "matrix.hh"
// #include "quaternion.hh"

class Camera {
public:
    Camera(const float x, const float y, const float z);

    void move(float amount);

    void strafe(float amount);

    void moveHeight(float amount);
    void pitch(float amount);

    void heading(float amount);

    void set();

private:
    Vec3 pos_;
    Matrix rot_;
};

#endif
