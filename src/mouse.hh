#ifndef MOUSE_HH
#define MOUSE_HH

#include <GL/glfw.h>

#include "inputdevice.hh"

class Mouse : public InputDevice<glfwGetMouseButton> {
public:
    Mouse();
    ~Mouse();

    enum MouseMode { ABSOLUTE, RELATIVE };

    void update();

    void changeMode(MouseMode mode);

    int x() const;
    int y() const;

private:
    MouseMode mode_;
    int x_;
    int y_;

    int oldX_;
    int oldY_;
};

#endif
