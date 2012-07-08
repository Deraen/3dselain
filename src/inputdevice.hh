#ifndef KEYBOARD_HH
#define KEYBOARD_HH

#include <map>

#include <GL/glfw.h>

#include "debug.hh"

template<int (*F)(int)>
class InputDevice {
public:
    InputDevice():
        binds_()
    {}

    ~InputDevice()
    {}

    void update()
    {
        for (auto& bind: binds_)
        {
            if (F(bind.first) == GLFW_PRESS)
            {
                if (bind.second == UP ||
                    bind.second == RELEASED)
                {
                    Debug::start() << bind.first << " pressed" << Debug::end();
                    // up -> down
                    bind.second = PRESSED;
                }
                else if (bind.second == PRESSED)
                {
                    // down -> down
                    bind.second = DOWN;
                }
            }
            else
            {
                if (bind.second == PRESSED ||
                    bind.second == DOWN)
                {
                    // down -> up
                    bind.second = RELEASED;
                }
                else if (bind.second == RELEASED)
                {
                    // up -> up
                    bind.second = UP;
                }
            }
        }
    }

    void registerKey(int key)
    {
        binds_.insert(std::pair<int, KeyStatus>(key, KeyStatus::UP));
    }

    bool keyPressed(int key) const
    {
        auto bind = binds_.find(key);
        if (bind != binds_.end())
        {
            return bind->second == PRESSED;
        }
        return false;
    }
    bool keyHold(int key) const
    {
        auto bind = binds_.find(key);
        if (bind != binds_.end())
        {
            return bind->second == PRESSED ||
                   bind->second == DOWN;
        }
        return false;
    }
    bool keyReleased(int key) const
    {
        auto bind = binds_.find(key);
        if (bind != binds_.end())
        {
            return bind->second == RELEASED;
        }
        return false;
    }

private:
    enum KeyStatus { UP, PRESSED, DOWN, RELEASED };

    std::map<int, KeyStatus> binds_;
};

typedef InputDevice<glfwGetKey> Keyboard;

#endif
