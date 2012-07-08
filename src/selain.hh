#ifndef SELAIN_HH
#define SELAIN_HH

#include <thread>

#include "camera.hh"
#include "engine.hh"

class Selain : public Engine {
public:
    Selain();
    ~Selain();

    void init();
    void end();

    void inputs();
    void display();
    void resize();

    void loader();

private:
    Camera camera_;
    bool wireframe_;
    std::thread loader_;
};

#endif
