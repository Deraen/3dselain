#include <GL3/gl3w.h>
#include <GL/glfw.h>

#include "debug.hh"
#include "engine.hh"

Engine::Engine(std::string title, int width, int windowHeight):
    mouse_(),
    keyboard_(),
    running_(false),
    title_(title),
    windowWidth_(width),
    windowHeight_(windowHeight_)
{}


Engine::~Engine()
{}


void Engine::start()
{
    running_ = true;

    Engine::init();

    while (running_)
    {
        keyboard_.update();
        mouse_.update();

        int oldWidth = windowWidth_;
        int oldHeight = windowHeight_;
        glfwGetWindowSize(&windowWidth_, &windowHeight_);
        if (windowWidth_ != oldWidth ||
            windowHeight_ != oldHeight)
        {
            Engine::resize();
        }

        inputs();
        display();
    }

    Engine::end();
}


void Engine::end()
{
    end();

    glfwTerminate();
}

void Engine::stop()
{
    running_ = false;
}


void Engine::resize()
{
    glViewport(0, 0, windowWidth_, windowHeight_);

    resize();
}


void Engine::init()
{
    glfwInit();
    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 8);
    if (!glfwOpenWindow(windowWidth_, windowHeight_, 8, 8, 8, 8, 8, 0, GLFW_WINDOW)) {
        Debug::start()[1] << "Kauhea virhe" << Debug::end();
        return; // fuu
    }
    glfwSetWindowTitle("JAA");

    if (gl3wInit()) {
        Debug::start()[0] << "failed to initialize OpenGL" << Debug::end();
        return;
    }

    if (!gl3wIsSupported(3, 0)) {
        Debug::start()[0] << "OpenGL 3.0 not supported" << Debug::end();
        return;
    }

    Debug::start()[0] << "OpenGL " << glGetString(GL_VERSION) << ", GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << Debug::end();

    glfwSwapInterval(1);

    glClearColor(0.0, 0.0, 0.2, 0.0); // Ruudun tyhjennysväri
    glEnable(GL_DEPTH_TEST);  // Z-testi

    glEnable(GL_CULL_FACE); // Ei piirretä kolmioita kameran takana?
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    glEnable(GL_MULTISAMPLE);

    init();
}


Mouse& Engine::mouse()
{
    return mouse_;
}


Keyboard& Engine::keyboard()
{
    return keyboard_;
}


