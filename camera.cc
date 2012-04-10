
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
    rot_(0.0, 0.0, 0.0, 0.0),
    heading_(180),
    pitch_(-15)
{
    rot_ = Quaternion(Vec3(0.0, 1.0, 0.0), heading_ * PI / 180) * Quaternion(Vec3(1.0, 0.0, 0.0), pitch_ * PI / 180);
}

void Camera::move(float amount) {
    Vec3 delta = rot_ * Vec3(0.0, 0.0, 1.0);
    pos_ += delta * amount;
}

void Camera::strafe(float amount) {
    Vec3 delta = rot_ * Vec3(1.0, 0.0, 0.0);
    pos_ += delta * amount;
}

void Camera::moveHeight(float amount) {
    Vec3 delta = rot_ * Vec3(0.0, 1.0, 0.0);
    pos_ += delta * amount;
}

void Camera::pitch(float amount) {
    pitch_ += amount;
    if (pitch_ < -90) pitch_ = -90;
    else if (pitch_ > 90) pitch_ = 90;
    else {
        Quaternion nrot(Vec3(1.0f, 0.0f, 0.0f), amount * PIOVER180);
        rot_ = rot_ * nrot;
    }
}

void Camera::heading(float amount) {
    heading_ += amount;

    Quaternion nrot(Vec3(0.0f, 1.0f, 0.0f), amount * PIOVER180);
    rot_ = nrot * rot_;
}

void Camera::set() {
    // XXX: kiihtyvyys

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<float>(width_) / static_cast<float>(height_), 0.1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(-pitch_, 1.0, 0.0, 0.0);
    glRotatef(-heading_, 0.0, 1.0, 0.0);
    // glRotatef(-rot_.getPitch() * 180 / PI, 1.0, 0.0, 0.0);
    // glRotatef(-rot_.getHeading() * 180 / PI, 0.0, 1.0, 0.0);

    glTranslatef(-pos_.x, -pos_.y, -pos_.z);
}

void Camera::setDisplayDim(const int w, const int h) {
    width_ = w;
    height_ = h;
}
