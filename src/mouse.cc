#include <GL/glfw.h>

#include "mouse.hh"

Mouse::Mouse():
    InputDevice(),
    x_(0),
    dx_(0),
    y_(0),
    dy_(0)
{}


Mouse::~Mouse()
{}


void Mouse::update()
{
    InputDevice::update();

    int oldX = x_;
    int oldY = y_;

    glfwGetMousePos(&x_, &y_);
    dx_ = oldX - x_;
    dy_ = oldY - y_;
}


int Mouse::x() const {
    return x_;
}


int Mouse::dx() const {
    return dx_;
}


int Mouse::y() const {
    return y_;
}


int Mouse::dy() const {
    return dy_;
}
