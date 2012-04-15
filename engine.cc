#include <map>
using std::pair;
#include <string>
using std::string;

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "drawable.hh"
#include "material.hh"
#include "animated.hh"

#include "sun.hh" // drawable, animated
#include "cube.hh" // drawable
#include "alusta.hh" // drawable
#include "solidmaterial.hh" // material

#include "camera.hh"

#include "engine.hh"

const unsigned int Engine::FPS = 60;
const unsigned int Engine::NS_PER_FRAME = pow(10, 9) / Engine::FPS;

Engine* Engine::instance_ = NULL;

void Engine::create() {
    assert(instance_ == NULL && "Engine-instansseja ei voi luoda useampia.");
    instance_ = new Engine;
}

Engine* Engine::instance() {
    if (instance_ != NULL) return instance_;
    assert(false && "Engine-instanssia ei ole luotu.");
}

Engine::Engine():
    camera_(0.0, 10.0, 0.0),
    timer_(),
    lightN(GL_LIGHT0)
{
    for (unsigned int i = 0; i <= 255; ++i) {
        keys_[i] = false;
    }

    glClearColor(0.0, 0.0, 0.2, 0.0); // Ruudun tyhjennysväri
    glEnable(GL_DEPTH_TEST);  // Z-testi

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);  // Sävytys: GL_FLAT / GL_SMOOTH

    objects_.push_back(new Sun(lightN));

    SolidMaterial* ground = new SolidMaterial;
    ground->setShinines(128);
    ground->setDiffuse(0.2, 1.0, 0.2);
    ground->setSpecular(1.0, 1.0, 1.0);
    materials_.insert(pair<string, Material*>("ground", ground));

    objects_.push_back(new Alusta(ground));

    SolidMaterial* red = new SolidMaterial;
    red->setSpecular(1.0, 0.5, 0.5);
    red->setDiffuse(1.0, 0.0, 0.0);
    materials_.insert(pair<string, Material*>("red", red));

    SolidMaterial* green = new SolidMaterial;
    green->setSpecular(0.5, 1.0, 0.5);
    green->setDiffuse(0.0, 1.0, 0.0);
    materials_.insert(pair<string, Material*>("green", green));

    SolidMaterial* blue = new SolidMaterial;
    blue->setSpecular(0.5, 0.0, 1.5);
    blue->setDiffuse(0.0, 0.0, 1.0);
    materials_.insert(pair<string, Material*>("blue", blue));

    objects_.push_back(new Cube(red, 20, 0, 0));
    objects_.push_back(new Cube(green, 0, 20, 0));
    objects_.push_back(new Cube(blue, 0, 0, 20));
}

Engine::~Engine() {
    for (unsigned int i = 0; i < objects_.size(); ++i) {
        // delete objects_.at(i); // XXX
    }
}

void Engine::HandleKeys() {
    if (instance()->keys_['q']) exit(0); // hmmm

    if (instance()->keys_['i']) instance()->camera_.move(-0.1);
    if (instance()->keys_['k']) instance()->camera_.move(0.1);
    if (instance()->keys_['j']) instance()->camera_.strafe(-0.1);
    if (instance()->keys_['l']) instance()->camera_.strafe(0.1);
    if (instance()->keys_['y']) instance()->camera_.moveHeight(0.1);
    if (instance()->keys_['h']) instance()->camera_.moveHeight(-0.1);
    if (instance()->keys_['w']) instance()->camera_.pitch(-1);
    if (instance()->keys_['s']) instance()->camera_.pitch(1);
    if (instance()->keys_['a']) instance()->camera_.heading(1);
    if (instance()->keys_['d']) instance()->camera_.heading(-1);
}

void Engine::display() {
    // Tyhjennetään ruutu ja Z-puskuri
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    instance()->camera_.set();

    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // wireframe
    for (unsigned int i = 0; i < instance()->objects_.size(); ++i) {
        glPushMatrix();

        Drawable* obj = instance()->objects_.at(i);

        // Jos piirrettävä toteuttaa Animated luokan niin kutsutaan sen
        // animate funktiota joka esim. siirtää sen sijaintia
        Animated* animate = dynamic_cast<Animated*>(obj);
        if (animate != NULL) animate->animate(instance()->timer_.millisecondsSinceStart() / 10.0);

        obj->draw();

        glPopMatrix();
    }

    // Vaihdetaan piirtopuskuri ja näkyvä puskuri
    glutSwapBuffers();
}

void Engine::resize(int newWidth, int newHeight) {
    // Asetetaan piirtoalue uudelleen, ja päivitetään leveys/korkeus; display-funktiossa
    //  kutsutaan gluPerspective-funktiota näistä lasketulla aspektisuhteella
    glViewport(0, 0, newWidth, newHeight);
    instance()->camera_.setDisplayDim(newWidth, newHeight);
}

void Engine::animate() {
    if (!instance()->timer_.ellapsed(Engine::NS_PER_FRAME)) {
        return;
    }

    HandleKeys();
    glutPostRedisplay();
}

void Engine::HandleKey(unsigned char key, int, int) {
    instance()->keys_[key] = true;
}

void Engine::HandleKeyRelease(unsigned char key, int, int) {

    instance()->keys_[key] = false;
}
