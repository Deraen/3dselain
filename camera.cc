
#include <iostream>
using std::cout;
using std::endl;

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>
#include "common.hh"
#include "vec3.hh"

#include "camera.hh"

Camera::Camera(const float x, const float y, const float z):
    pos_(x, y, z),
    width_(0),
    height_(0),
    rot_(4, 4)
{
    rot_ << 1 << 0 << 0 << 0
         << 0 << 1 << 0 << 0
         << 0 << 0 << 1 << 0
         << 0 << 0 << 0 << 1;

    heading(45);
    pitch(-25);
}

void Camera::move(float amount) {
    Matrix delta(0.0, 0.0, 1.0);
    delta = rot_ * delta;
    delta *= amount;
    pos_ += delta;
}

void Camera::strafe(float amount) {
    Matrix delta(1.0, 0.0, 0.0);
    delta = rot_ * delta;
    delta *= amount;
    pos_ += delta;
}

void Camera::moveHeight(float amount) {
    Matrix delta(0.0, 1.0, 0.0);
    delta = rot_ * delta;
    delta *= amount;
    pos_ += delta;
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

void Camera::set() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<float>(width_) / static_cast<float>(height_), 0.1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMultMatrixd(rot_.data());

    glTranslatef(-pos_.get(X), -pos_.get(Y), -pos_.get(Z));
}

void Camera::setDisplayDim(const int w, const int h) {
    width_ = w;
    height_ = h;
}
