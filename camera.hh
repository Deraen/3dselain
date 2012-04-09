#include <iostream>
using std::cout;
using std::endl;

#include <GL/gl.h>
#include <GL/glut.h>

#include "vec3.hh"

class Camera {
public:
    Camera(const float x, const float y, const float z):
        pos_(x, y, z),
        pitch_(-15),
        heading_(180)
    {}

    void move(float amount) {
        // suunta: 0 0 1
        // eka heading käännös (y ympäri)
        // sitten pitch (x ympäri)
        // Vec3 delta(amount * sin(heading_ * PI / 180),
        //            amount * sin(pitch_ * PI / 180),
        //            amount * cos(heading_ * PI / 180));
        // delta /= 10*delta.length();
        Vec3 delta(0.0, 0.0, amount);
        delta.rotateY(heading_);
        pos_ += delta;

        cout << "||(" << delta.x << ", " << delta.y << ", " << delta.z << ")|| = " << sqrt(pow(delta.x, 2) + pow(delta.y, 2) + pow(delta.z, 2)) << endl;
    }

    void strafe(float amount) {
        Vec3 delta(amount * sin((heading_ - 90) * PI / 180),
                   0,
                   amount * cos((heading_ - 90) * PI / 180));
        pos_ += delta;
    }

    void moveHeight(float amount) {
        // suunta: 0 1 0
        // käännetään eka pitch mukaan (x ympäri)
        // sitten heading (y ympäri)
        Vec3 delta(0, amount, 0);
        // delta.rotateX(pitch_);
        // delta.rotateY(heading_);
        pos_ += delta;
    }

    void pitch(float amount) {
        pitch_ += amount;
        if (pitch_ < -90) pitch_ = -90;
        if (pitch_ > 90) pitch_ = 90;
    }

    void heading(float amount) {
        heading_ += amount;
        heading_ = fmod(heading_, (float)360.0);
    }

    void set() {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, static_cast<float>(width_) / static_cast<float>(height_), 0.1, 100);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glRotatef(-pitch_, 1.0, 0.0, 0.0);
        glRotatef(-heading_, 0.0, 1.0, 0.0);

        glTranslatef(-pos_.x, -pos_.y, -pos_.z);
    }

    void setDisplayDim(const int w, const int h) {
        width_ = w;
        height_ = h;
    }

private:
    Vec3 pos_;

    int width_;
    int height_;

    float pitch_;
    float heading_;
};