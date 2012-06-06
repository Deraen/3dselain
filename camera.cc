
#include <iostream>
using std::cout;
using std::endl;

#include <GL/glew.h>
// #define GL_GLEXT_PROTOTYPES
// #include <GL/gl.h>
// #include <GL/glut.h>
#include "common.hh"
#include "vec3.hh"

#include "camera.hh"

Camera::Camera(const float x, const float y, const float z):
    pos_(x, y, z),
    delta_(0.0, 0.0, 0.0),
    rot_(4, 4),
    projection_(4, 4),
    location_(4, 4)
{
    rot_ << 1 << 0 << 0 << 0
         << 0 << 1 << 0 << 0
         << 0 << 0 << 1 << 0
         << 0 << 0 << 0 << 1;

    heading(45);
    pitch(-25);
}

void Camera::applyMovement(const Vec3& move) {
    pos_ += move;
    delta_ = Vec3(0.0, 0.0, 0.0);
}

Vec3 Camera::getMovement() const { return delta_; }

Vec3 Camera::getPos() const { return pos_; }

Vec3 Camera::getVector() const {
    Vec3 suunta(0.0, 0.0, -1.0);
    suunta = rot_ * suunta;
    return suunta.normalize();
}

void Camera::move(float amount) {
    Vec3 suunta(0.0, 0.0, 1.0);
    suunta = rot_ * suunta;
    suunta *= amount;
    delta_ += suunta;
}

void Camera::strafe(float amount) {
    Vec3 suunta(1.0, 0.0, 0.0);
    suunta = rot_ * suunta;
    suunta *= amount;
    delta_ += suunta;
}

void Camera::moveHeight(float amount) {
    Vec3 suunta(0.0, 1.0, 0.0);
    suunta = rot_ * suunta;
    suunta *= amount;
    delta_ += suunta;
}

void Camera::pitch(float amount) {
    amount = deg2rad(amount);

    Matrix rx(4, 4);
    rx << 1 << 0           << 0            << 0
       << 0 << cos(amount) << -sin(amount) << 0
       << 0 << sin(amount) << cos(amount)  << 0
       << 0 << 0           << 0            << 1;

    // Ei suoriteta pyöritystä jos sen seurauksena katsottaisiin
    // taaksepäin (pitch lukittu -90 ja 90 välille)
    Matrix newrot = rot_ * rx;
    if (newrot.get(1, 1) >= 0) rot_ = newrot;
}

void Camera::heading(float amount) {
    amount = deg2rad(amount);

    Matrix ry(4, 4);
    ry << cos(amount)  << 0 << sin(amount) << 0
       << 0            << 1 << 0           << 0
       << -sin(amount) << 0 << cos(amount) << 0
       << 0            << 0 << 0           << 1;

    rot_ = ry * rot_;
}

// void Camera::set() {
//     glMatrixMode(GL_MODELVIEW);
//     glLoadIdentity();

//     glMultMatrixd(rot_.data());

//     glTranslatef(-pos_.x, -pos_.y, -pos_.z);
// }

float* Camera::projection(unsigned int w, unsigned int h) {
    float aspect = static_cast<float>(w) / h;
    float fovY = 45.0;
    float fH = tan(fovY / 360 * PI) * 1;
    float fW = fH * aspect;

    const float near = 1.0;
    const float far = 500;

    float a = 0; // (fW + (-fW)) / (fW - (-fW))
    float b = 0; // (fH + (-fH)) / (fH - (-fH))
    float c = (far + near) / (far - near);
    float d = (2 * far * near) / (far -near);

    float* r = new float[16];
    r[0] = near / (2 * fW); r[1] = 0;               r[2] = a;     r[3] = 0;
    r[4] = 0;               r[5] = near / (2 * fH); r[6] = b;     r[7] = 0;
    r[8] = 0;               r[9] = 0;               r[10] = c;    r[11] = d;
    r[12] = 0;              r[13] = 0;              r[14] = -1.0; r[15] = 0;

    return r;
}
