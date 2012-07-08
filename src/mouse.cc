#include <GL/glfw.h>

#include "common.hh"
#include "mouse.hh"

Mouse::Mouse():
    InputDevice(),
    mode_(ABSOLUTE),
    x_(0),
    y_(0),
    oldX_(0),
    oldY_(0)
{}


Mouse::~Mouse()
{}


void Mouse::update()
{
    InputDevice::update();

    glfwGetMousePos(&x_, &y_);

    if (mode_ == RELATIVE)
    {
        // XXX: Should use window size from Engine
        int w, h;
        glfwGetWindowSize(&w, &h);

        x_ -= divide(w, 2);
        y_ -= divide(h, 2);

        glfwSetMousePos(divide(w, 2), divide(h, 2));
    }
}


void Mouse::changeMode(MouseMode mode)
{
    mode_ = mode;

    if (mode_ == ABSOLUTE)
    {
        glfwSetMousePos(oldX_, oldY_);
    }
    else if (mode == RELATIVE)
    {
        glfwGetMousePos(&oldX_, &oldY_);
        // XXX:
        int w, h;
        glfwGetWindowSize(&w, &h);
        glfwSetMousePos(divide(w, 2), divide(h, 2));
    }

    update();
}


int Mouse::x() const {
    return x_;
}


int Mouse::y() const {
    return y_;
}
