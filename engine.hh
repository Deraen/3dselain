#ifndef ENGINE_HH
#define ENGINE_HH

#include <map>
#include <vector>
#include <string>
#include <cassert>

#include <sys/time.h>

class Drawable;
class Material;

#include "camera.hh"
#include "timer.hh"

class Engine {
public:

    ~Engine();

    static void HandleKeys();

    static void display();

    static void resize(int newWidth, int newHeight);

    static void animate();

    static void HandleKey(unsigned char key, int, int);

    static void HandleKeyRelease(unsigned char key, int, int);

    static void create();

    static Engine* instance();

    static const unsigned int FPS;
    static const unsigned int NS_PER_FRAME;

private:

    Engine();

    static Engine* instance_;

    Camera camera_;
    Timer timer_;
    bool keys_[256];
    double startTime_;
    std::map<std::string, Material*> materials_;
    std::vector<Drawable*> objects_;

    GLenum lightN;
};

#endif
