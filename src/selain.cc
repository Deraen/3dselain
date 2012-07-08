#include <GL3/gl3w.h>
#include <GL/glfw.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "animated.hh"
#include "lumploader.hh"
#include "scene.hh"
#include "manager.hh"
#include "selain.hh"

Selain::Selain():
    Engine("3DSelain", 800, 600),
    camera_(6540.0, 71.0, -4880.0),
    wireframe_(false)
{}


Selain::~Selain()
{}


void Selain::init()
{
    Manager& manager = Manager::instance();

    manager.addShader("lightning");
    manager.getShader("lightning")->addVertexShader("assets/shaders/lightning.vert");
    manager.getShader("lightning")->addFragmentShader("assets/shaders/lightning.frag");
    manager.getShader("lightning")->load();
    manager.getShader("lightning")->begin();

    manager.addObject("water", new Scene("assets/obj/water/wa-etelasatama_hd.obj"));
    manager.addObject("water", new Scene("assets/obj/water/wa-katajanokka_etela_hd.obj"));
    manager.addObject("water", new Scene("assets/obj/water/wa-kaivopuisto_hd.obj"));
    manager.addObject("water", new Scene("assets/obj/water/wa-taivallahti_hd.obj"));
    manager.addObject("water", new Scene("assets/obj/water/wa-eira_hd.obj"));
    manager.addObject("water", new Scene("assets/obj/water/wa-elaintarhanlahti_hd.obj"));
    manager.addObject("water", new Scene("assets/obj/water/wa-jatkasaari_hd.obj"));
    manager.addObject("water", new Scene("assets/obj/water/wa-ullanlinna_hd.obj"));
    manager.addObject("water", new Scene("assets/obj/water/wa-katajanokka_pohjoinen_hd.obj"));
    manager.addObject("water", new Scene("assets/obj/water/wa-sornaistensatama_hd.obj"));
    manager.addObject("water", new Scene("assets/obj/water/wa-ruoholahti_hd.obj"));
    manager.addObject("water", new Scene("assets/obj/water/wa-toolonlahti_hd.obj"));
    manager.addObject("water", new Scene("assets/obj/water/wa-hietalahti_hd.obj"));
    manager.addObject("water", new Scene("assets/obj/water/wa-sompasaarensalmi_hd.obj"));

    LumpLoader lumps("assets/obj/000-helsinki-2009_stripped.lpf");
    lumps.load();

    loader_ = std::thread(&Selain::loader, this);

    keyboard().registerKey('1');
    keyboard().registerKey('4');
    keyboard().registerKey('5');
    keyboard().registerKey('6');

    keyboard().registerKey('W');
    keyboard().registerKey('A');
    keyboard().registerKey('S');
    keyboard().registerKey('D');
    keyboard().registerKey(' ');

    keyboard().registerKey(GLFW_KEY_ESC);

    mouse().registerKey(GLFW_MOUSE_BUTTON_LEFT);
    mouse().registerKey(GLFW_MOUSE_BUTTON_RIGHT);
}


void Selain::end()
{
    loader_.join();

    Manager::instance().unload();
}

void Selain::inputs()
{
    if (keyboard().keyPressed('1')) {
        wireframe_ = !wireframe_;
    }

    if (keyboard().keyPressed('4')) Texture::enabled = !Texture::enabled;
    if (keyboard().keyPressed('5')) Debug::start()[1] << "Kameran koordinaatti " << camera_.getPos() << Debug::end();
    // if (keyboard().keyPressed('6')) collisionDetection_ = !collisionDetection_;

    // Alla olevat siirrot tallentuvat kameralle
    if (keyboard().keyHold('W') ||
        mouse().keyHold(GLFW_MOUSE_BUTTON_LEFT)) camera_.move(1);
    if (keyboard().keyHold('S')) camera_.move(-1);
    if (keyboard().keyHold('A')) camera_.strafe(-1);
    if (keyboard().keyHold('D')) camera_.strafe(1);
    // Yhteenlasketut siirrot
    glm::vec3 movement = camera_.getMovement();

    if (keyboard().keyHold(' ')) movement += glm::vec3(0.0, 1.0, 0.0);
    // Ollaanko törmäämässä johonkin?
    // törmäys päivittää liike vektoria törmäyksen mukaisesti
    // for (unsigned int i = 0; i < manager.objectCount() && collisionDetection_; ++i) {
        // manager.getObject(i)->collision(camera_.getPos(), movement);
    // }

    // Suoritetaan muutettu siirto
    camera_.applyMovement(movement);

    if (mouse().keyPressed(GLFW_MOUSE_BUTTON_RIGHT))
    {
        mouse().changeMode(Mouse::MouseMode::RELATIVE);
    }

    if (mouse().keyHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        double heading = -mouse().x() / divide(windowWidth(), 2) * 7.0;
        double pitch   = -mouse().y() / divide(windowHeight(), 2) * 7.0;
        camera_.rotate(pitch, heading);
    }

    if (mouse().keyReleased(GLFW_MOUSE_BUTTON_RIGHT))
    {
        mouse().changeMode(Mouse::MouseMode::ABSOLUTE);
    }


    if (keyboard().keyHold(GLFW_KEY_ESC) ||
        !glfwGetWindowParam(GLFW_OPENED))
    {
        stop();
    }
}


void Selain::resize()
{
    Shader* shader = Manager::instance().getShader("lightning");

    glm::mat4 projection = glm::perspective(45.0f, (1.0f * windowWidth()) / windowHeight(), 5.0f, 1000.0f);
    glUniformMatrix4fv(shader->uniformLoc("projection"), 1, GL_FALSE, glm::value_ptr(projection));
}


void Selain::display()
{
    Manager& manager = Manager::instance();

    if (wireframe_) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
}


void Selain::loader() {
    std::chrono::milliseconds duration(50); // 20 kertaa sekuntissa
    Manager& manager = Manager::instance();

    while (running()) {
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
