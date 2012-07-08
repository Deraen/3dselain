#ifndef ENGINE_HH
#define ENGINE_HH

#include <atomic>
#include <string>

#include "mouse.hh"
#include "keyboard.hh"

class Engine {
public:
    Engine(std::string title, int width, int height);
    ~Engine();

    virtual void init();
    virtual void end();
    virtual void inputs() = 0;
    virtual void display() = 0;
    virtual void resize();

    void start();
    void stop();

    int windowWidth() const { return windowWidth_; }
    int windowHeight() const { return windowHeight_; }
    bool running() const { return running_; }

    Mouse& mouse();
    Keyboard& keyboard();

private:
    Mouse mouse_;
    Keyboard keyboard_;
    std::atomic<bool> running_;

    std::string title_;
    int windowWidth_;
    int windowHeight_;
};

#endif
