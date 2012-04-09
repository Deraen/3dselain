
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
    rot_(0.0, 0.0, -1.0, 0.0)//,
    // heading_(180),
    // pitch_(-15)
{
}

void Camera::move(float amount) {
    // suunta: 0 0 1
    // eka heading käännös (y ympäri)
    // sitten pitch (x ympäri)
    // Vec3 delta(amount * sin(angle2rad(heading_)),
    //            amount * sin(angle2rad(pitch_)),
    //            amount * cos(angle2rad(heading_)) * cos(angle2rad(pitch_)));
    // Vec3 delta(0.0, 0.0, amount);
    // delta.rotateY(heading_);
    // pos_ += delta;
    // Matrix delta(0.0, 0.0, -amount);
    // delta = rot_ * delta;
    Vec3 delta = rot_ * Vec3(0.0, 0.0, -amount);
    pos_ += delta;

    cout << "||(" << delta.x << ", " << delta.y << ", " << delta.z << ")|| = " << sqrt(pow(delta.x, 2) + pow(delta.y, 2) + pow(delta.z, 2)) << endl;
    // cout << "||(" << delta.get(X) << ", " << delta.get(Y) << ", " << delta.get(Z) << ")|| = " << sqrt(pow(delta.get(X), 2) + pow(delta.get(Y), 2) + pow(delta.get(Z), 2)) << endl;
}

void Camera::strafe(float amount) {
    // Vec3 delta(amount * sin((heading_ - 90) * PI / 180),
    //            0,
    //            amount * cos((heading_ - 90) * PI / 180));
    // pos_ += delta;
    Vec3 delta = rot_ * Vec3(amount, 0.0, 0.0);
    pos_ += delta;
}

void Camera::moveHeight(float amount) {
    // suunta: 0 1 0
    // käännetään eka pitch mukaan (x ympäri)
    // sitten heading (y ympäri)
    // Vec3 delta(0, amount, 0);
    // delta.rotateX(pitch_);
    // delta.rotateY(heading_);
    // pos_ += delta;
    Vec3 delta = rot_ * Vec3(0.0, amount, 0.0);
    pos_ += delta;
}

void Camera::pitch(float amount) {
    // pitch_ += amount;
    // if (pitch_ < -90) pitch_ = -90;
    // if (pitch_ > 90) pitch_ = 90;
    //     // Pitch - x akselin ympäri
    // float rad = amount * PI / 180;
    // Matrix rx(4, 4);
    // rx << 1 << 0         << 0        << 0
    //    << 0 << cos(rad)  << sin(rad) << 0
    //    << 0 << -sin(rad) << cos(rad) << 0
    //    << 0 << 0         << 0        << 1;

    // rot_ = rx * rot_;
    Quaternion nrot(Vec3(1.0f, 0.0f, 0.0f), amount * PIOVER180);
    rot_ = rot_ * nrot;
}

void Camera::heading(float amount) {
    // heading_ += amount;
    // heading_ = fmod(heading_, (float)360.0);

    // // Heading - pyöritetään y akselin ympäri
    // float rad = amount * PI / 180;
    // Matrix ry(4, 4);
    // ry << cos(rad)  << 0 << sin(rad) << 0
    //    << 0         << 1 << 0        << 0
    //    << -sin(rad) << 0 << cos(rad) << 0
    //    << 0         << 0 << 0        << 1;

    // rot_ = ry * rot_;
    Quaternion nrot(Vec3(0.0f, 1.0f, 0.0f), amount * PIOVER180);
    rot_ = nrot * rot_;
}

void Camera::set() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<float>(width_) / static_cast<float>(height_), 0.1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float pitch = rot_.getAxisAngle(Vec3(1.0, 0.0, 0.0)) * 180 / PI;
    float heading = rot_.getAxisAngle(Vec3(0.0, 1.0, 0.0)) * 180 / PI;
    cout << pitch << endl;
    cout << heading << endl;
    glRotatef(-pitch, 1.0, 0.0, 0.0);
    glRotatef(-heading, 0.0, 1.0, 0.0);

    glTranslatef(-pos_.x, -pos_.y, -pos_.z);
}

void Camera::setDisplayDim(const int w, const int h) {
    width_ = w;
    height_ = h;
}
