#ifndef MOUSE_HH
#define MOUSE_HH

#include <GL/glfw.h>

#include "keyboard.hh"

class Mouse : public InputDevice<glfwGetMouseButton> {
public:
    Mouse();
    ~Mouse();

    void update();

    int x() const;
    int y() const;
    int dx() const;
    int dy() const;

private:
    int x_;
    int dx_;
    int y_;
    int dy_;
};

#endif
