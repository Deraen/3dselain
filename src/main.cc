#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

#include <GL3/gl3w.h>
#include <GL/glfw.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common.hh"
#include "manager.hh"
#include "camera.hh"
#include "scene.hh"
#include "animated.hh"
#include "shader.hh"
#include "lumploader.hh"

// --- VAKIOT ---
const int DEF_WINDOW_WIDTH = 800;
const int DEF_WINDOW_HEIGHT = 600;
const std::chrono::milliseconds MS_PER_FRAME(16); // piirretään frame noin 60 kertaa sekuntissa

// --- GLOBAALIT ---
Camera camera_(6540.0, 71.0, -4880.0);
std::mutex mouseMutex;
int dX_, dY_, oldX_, oldY_;
bool wireframe_ = false;
bool collisionDetection_ = true;
int running_ = GL_TRUE;

void init() {
    Manager& manager = Manager::instance();

    manager.addShader("lightning");
    manager.getShader("lightning")->addVertexShader("assets/shaders/lightning.vert");
    manager.getShader("lightning")->addFragmentShader("assets/shaders/lightning.frag");
    manager.getShader("lightning")->load();
    manager.getShader("lightning")->begin();

    manager.addObject("st-keskusta", new Scene("assets/obj/st-keskusta.obj"));

    LumpLoader lumps("assets/obj/000-helsinki-2009_nowa.lpf");
    lumps.load();

    glClearColor(0.0, 0.0, 0.2, 0.0); // Ruudun tyhjennysväri
    glEnable(GL_DEPTH_TEST);  // Z-testi

    glEnable(GL_CULL_FACE); // Ei piirretä kolmioita kameran takana?
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    glEnable(GL_MULTISAMPLE);
}

void handleKey(int key, int action) {
    if (action == GLFW_PRESS) {
        if (key == '1') {
            wireframe_ = !wireframe_;
            if (wireframe_) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
            else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (key == '4') Texture::enabled = !Texture::enabled;
        if (key == '5') Debug::start()[1] << "Kameran koordinaatti " << camera_.getPos() << Debug::end();
        if (key == '6') collisionDetection_ = !collisionDetection_;
    }
}

void mouse(int key, int action) {
    if (key == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            std::lock_guard<std::mutex> lock(mouseMutex);
            // talletaan hiiren sijainti
            int x, y;
            glfwGetMousePos(&x, &y);
            oldX_ = x;
            oldY_ = y;
            dX_ = 0;
            dY_ = 0;
            glfwGetWindowSize(&x, &y);
            glfwSetMousePos(divide(x, 2), divide(y, 2));
        } else if (action == GLFW_RELEASE) {
            glfwSetMousePos(oldX_, oldY_);
        }
    }
}

void motion(int x, int y) {
    if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
        // talletetaan koordinaatit jotta voidaan kääntää kameraa
        // kun ensikerran piirretään ruutua
        int w, h;
        glfwGetWindowSize(&w, &h);
        dX_ = x - divide(w, 2);
        dY_ = y - divide(h, 2);

        glfwGetWindowSize(&w, &h);
        glfwSetMousePos(divide(w, 2), divide(h, 2));
    }
}

void resize(int w, int h) {
    glViewport(0, 0, w, h);

    Shader* shader = Manager::instance().getShader("lightning");

    glm::mat4 projection = glm::perspective(45.0f, (1.0f * w) / h, 5.0f, 1000.0f);
    glUniformMatrix4fv(shader->uniformLoc("projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void loader() {
    std::chrono::milliseconds duration(50); // 20 kertaa sekuntissa
    Manager& manager = Manager::instance();

    while (running_) {
        glm::vec3 pos = camera_.getPos();

        for (unsigned int i = 0; i < manager.objectCount(); ++i) {
            Scene* scene = dynamic_cast<Scene*>(manager.getObject(i));
            if (scene) {
                scene->visibility(pos);
            }
        }

        std::this_thread::sleep_for(duration);
    }
}

void handleKeys() {
    std::chrono::milliseconds duration(10); // 100 kertaa sekuntissa
    Manager& manager = Manager::instance();

    while (running_) {

        // Alla olevat siirrot tallentuvat kameralle
        if (glfwGetKey('W') || glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) camera_.move(1);
        if (glfwGetKey('S')) camera_.move(-1);
        if (glfwGetKey('A')) camera_.strafe(-1);
        if (glfwGetKey('D')) camera_.strafe(1);
        // Yhteenlasketut siirrot
        glm::vec3 movement = camera_.getMovement();

        if (glfwGetKey(' ')) movement += glm::vec3(0.0, 1.0, 0.0);
        // Ollaanko törmäämässä johonkin?
        // törmäys päivittää liike vektoria törmäyksen mukaisesti
        for (unsigned int i = 0; i < manager.objectCount() && collisionDetection_; ++i) {
            manager.getObject(i)->collision(camera_.getPos(), movement);
        }

        // Suoritetaan muutettu siirto
        camera_.applyMovement(movement);

        if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
            std::lock_guard<std::mutex> lock(mouseMutex);

            int x, y;
            glfwGetWindowSize(&x, &y);
            // 3 astetta
            double heading = -dX_ / divide(x, 2) * 7.0;
            double pitch   = -dY_ / divide(y, 2) * 7.0;
            dX_ = 0;
            dY_ = 0;
            camera_.rotate(pitch, heading);
            glfwGetWindowSize(&x, &y);
            glfwSetMousePos(x / 2, y / 2);
        }

        std::this_thread::sleep_for(duration);
    }
}

void display() {
    Manager& manager = Manager::instance();
    auto time_now = std::chrono::steady_clock::now();

    while (running_) {
        time_now = std::chrono::steady_clock::now();

        // Tyhjennetään ruutu ja Z-puskuri
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera_.setup();

        for (unsigned int i = 0; i < manager.objectCount(); ++i) {
            Drawable* obj = manager.getObject(i);

            // Jos piirrettävä toteuttaa Animated luokan niin kutsutaan sen
            // animate funktiota joka esim. siirtää sen sijaintia
            Animated* animate = dynamic_cast<Animated*>(obj);
            if (animate != NULL) animate->animate(glfwGetTime());

            Scene* scene = dynamic_cast<Scene*>(obj);
            if (scene != NULL) {
                // Siirtää tiedostosta ladatut meshit näytönohjaimen muistiin
                scene->loadVao();
                // Poistaa meshit näytönohjaimen muistista jos merkitty poistettavaksi
                scene->unloadVao();
            }

            obj->draw();
        }

        // Vaihdetaan piirtopuskuri ja näkyvä puskuri
        glfwSwapBuffers();

        std::this_thread::sleep_until(time_now + MS_PER_FRAME);

        running_ = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
    }

    Debug::start()[1] << "Ikkuna suljettu tai ESC." << Debug::end();
}

int main(int argc, char *argv[]) {
    glfwInit();
    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
    glfwOpenWindow(DEF_WINDOW_WIDTH, DEF_WINDOW_HEIGHT, 8, 8, 8, 8, 32, 0, GLFW_WINDOW);
    glfwSetWindowTitle("JAA");

    if (gl3wInit()) {
        Debug::start()[0] << "failed to initialize OpenGL" << Debug::end();
        return EXIT_FAILURE;
    }

    if (!gl3wIsSupported(3, 0)) {
        Debug::start()[0] << "OpenGL 3.0 not supported" << Debug::end();
        return EXIT_FAILURE;
    }

    Debug::start()[0] << "OpenGL " <<  glGetString(GL_VERSION) << ", GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << Debug::end();

    init();

    std::thread loaderThread(loader);
    std::thread inputThread(handleKeys);

    glfwSetKeyCallback(handleKey);
    glfwSetMouseButtonCallback(mouse);
    glfwSetMousePosCallback(motion);
    glfwSetWindowSizeCallback(resize);

    // Käynnistää silmukan
    try {
        display();
    } catch(FatalException e) {
        Debug::start()[0] << "Lopetetaan ohjelma koska: " << e.what() << Debug::end();
    }

    loaderThread.join();
    inputThread.join();

    // Halutaan ajaa deletet main-threadissä koska OpenGL
    Manager::instance().unload();

    glfwTerminate();

    return 0;
}
