#ifndef ENGINE_HH
#define ENGINE_HH

#include <map>
#include <vector>
#include <string>
#include <cassert>

class Drawable;
class Material;

#include "camera.hh"

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

private:

    Engine();

    static Engine* instance_;

    Camera camera_;
    bool keys_[256];
    float time_;
    std::map<std::string, Material*> materials_;
    std::vector<Drawable*> objects_;

    GLenum lightN;
};

#endif
