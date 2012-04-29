/*
-gldebug käynnistysvipu saattaa auttaa opengl virheiden etsinnässä.

Vaadittujen painikkeiden lisäksi:
'<' - Wireframe päälle/pois.
'Z' - Kolmioiden normaalien piirto.
'X' - Kolmioiden keskipisteiden piirto.
Space - Ylöspäin kamerasta riippumatta.
*/

#include <iostream>
#include <vector>
using std::vector;

// Define jotain jänniä juttuja varten
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>

#include "camera.hh"
#include "drawable.hh"

// --- VAKIOT ---
const int DEF_WINDOW_WIDTH = 800;
const int DEF_WINDOW_HEIGHT = 600;
const unsigned int FPS = 60;
const unsigned int MS_PER_FRAME = pow(10, 3) / FPS;
const unsigned int NUM_OF_KEYS = sizeof(char);
const unsigned int NUM_OF_BUTTONS = 3;

// --- GLOBAALIT ---
Camera camera_(0.0, 10.0, 0.0);
bool keys_[256];
bool buttons_[3];
vector<Drawable*> objects_;
unsigned int windowWidth_, windowHeight_, rotX_, rotY_, oldX_, oldY_;
bool wireframe_ = false;
double secondsSinceStart_ = 0.0; // hyvin epätarkka arvio (glutTimer...)

// Eri objekteja varten
#include "sun.hh"
#include "alusta.hh"
#include "solidmaterial.hh"
#include "cube.hh"
void init() {
    for (unsigned int i = 0; i <= NUM_OF_KEYS; ++i) {
        keys_[i] = false;
    }
    for (unsigned int i = 0; i <= NUM_OF_BUTTONS; ++i) {
        buttons_[i] = false;
    }

    glClearColor(0.0, 0.0, 0.2, 0.0); // Ruudun tyhjennysväri
    glEnable(GL_DEPTH_TEST);  // Z-testi

    glEnable(GL_CULL_FACE); // Ei piirretä kolmioita kameran takana?
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    glEnable(GL_LIGHTING);
    // glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);  // Sävytys: GL_FLAT / GL_SMOOTH

    objects_.push_back(new Sun);
    objects_.push_back(new Alusta);
    objects_.push_back(new Cube(1.0, 0.5, 0.5, 20, 0, 0));
    objects_.push_back(new Cube(0.5, 1.0, 0.5, 0, 20, 0));
    objects_.push_back(new Cube(0.5, 0.5, 1.0, 0, 0, 20));
}

void destroy() {
    for (unsigned int i = 0; i < objects_.size(); ++i) {
        delete objects_.at(i);
    }
}

void handleKey(unsigned char key, int, int) {
    if (key == '<') wireframe_ = !wireframe_;
    if (key == 'z') ObjReader::drawNormals_ = !ObjReader::drawNormals_;
    if (key == 'x') ObjReader::drawFaceCenters_ = !ObjReader::drawFaceCenters_;
    keys_[key] = true;
}

void handleKeyRelease(unsigned char key, int, int) {

    keys_[key] = false;
}

void display() {
    // Tyhjennetään ruutu ja Z-puskuri
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<float>(windowWidth_) / static_cast<float>(windowHeight_), 0.1, 100);

    camera_.set();

    if (wireframe_) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // wireframe
    else glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    for (unsigned int i = 0; i < objects_.size(); ++i) {
        glPushMatrix();

        Drawable* obj = objects_.at(i);

        // Jos piirrettävä toteuttaa Animated luokan niin kutsutaan sen
        // animate funktiota joka esim. siirtää sen sijaintia
        Animated* animate = dynamic_cast<Animated*>(obj);
        if (animate != NULL) animate->animate(secondsSinceStart_);

        obj->draw();

        glPopMatrix();
    }

    // Vaihdetaan piirtopuskuri ja näkyvä puskuri
    glutSwapBuffers();
}

void resize(int newWidth, int newHeight) {
    glViewport(0, 0, newWidth, newHeight);
    windowWidth_ = newWidth;
    windowHeight_ = newHeight;
}

void handleKeys() {
    if (keys_['q']) exit(0); // hmmm

    // törmäyksen tarkistus näin tyhmään paikkaan
    // Camera oldCamera = camera_;

    // Alla olevat siirrot tallentuvat kameralle
    if (keys_['i'] || buttons_[GLUT_LEFT_BUTTON]) camera_.move(-0.1);
    if (keys_['k']) camera_.move(0.1);
    if (keys_['j']) camera_.strafe(-0.1);
    if (keys_['l']) camera_.strafe(0.1);
    if (keys_['y']) camera_.moveHeight(0.1);
    if (keys_['h']) camera_.moveHeight(-0.1);
    // Yhteenlasketut siirrot
    Vec3 movement = camera_.getMovement();

    if (keys_[' ']) movement += Vec3(0.0, 0.2, 0.0);
    // movement += Vec3(0.0, -0.1, 0.0);
    // Ollaanko törmäämässä johonkin?
    // for (unsigned int i = 0; i < objects_.size(); ++i) {
    //     objects_.at(i)->collision(camera_.getPos(), movement);
    // }
    objects_.at(1)->collision(camera_.getPos(), movement);

    // Suoritetaan muutettu siirto
    camera_.applyMovement(movement);

    if (keys_['w']) camera_.pitch(-1);
    if (keys_['s']) camera_.pitch(1);
    if (keys_['a']) camera_.heading(1);
    if (keys_['d']) camera_.heading(-1);

    // oikea hiiren painike
    if (buttons_[GLUT_RIGHT_BUTTON]) {
        double w = static_cast<double>(windowWidth_) / 2;
        double h = static_cast<double>(windowHeight_) / 2;
        // käänny korkeintaan 1° per ruudun piirto
        double heading = -(static_cast<double>(rotX_) - w) / w * 1;
        double pitch   = -(static_cast<double>(rotY_) - h) / h * 1;
        camera_.heading(heading);
        camera_.pitch(pitch);
    }
}

void mouse(int button, int state, int x, int y) {
    if (button < GLUT_LEFT_BUTTON
     || button > GLUT_RIGHT_BUTTON) return;

    if (state == GLUT_DOWN) {
        buttons_[button] = true;
        // talletaan hiiren sijainti
        oldX_ = x;
        oldY_ = y;
        // hiiri keskelle jotta helpompi pyörittää
        glutWarpPointer(windowWidth_ / 2, windowHeight_ / 2);
    } else if (state == GLUT_UP) {
        buttons_[button] = false;
        // palautetaa hiiri vanhaan paikkaan
        glutWarpPointer(oldX_, oldY_);
    }
}

void motion(int x, int y) {
    if (buttons_[GLUT_RIGHT_BUTTON]) {
        // talletetaan koordinaatit jotta voidaan kääntää kameraa
        // kun ensikerran piirretään ruutua
        rotX_ = x;
        rotY_ = y;
    }
}

void animate(int a = 0) {
    handleKeys();
    glutPostRedisplay();

    secondsSinceStart_ += static_cast<double>(MS_PER_FRAME) / pow(10, 3);

    glutTimerFunc(MS_PER_FRAME, animate, 0);
}

int main(int argc, char *argv[]) {
    // Alustetan GLUT ja luodaan ikkuna
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(DEF_WINDOW_WIDTH, DEF_WINDOW_HEIGHT);
    glutCreateWindow("TrollGL");

    glutIgnoreKeyRepeat(GLUT_KEY_REPEAT_ON);

    init();

    glutKeyboardFunc(handleKey);
    glutKeyboardUpFunc(handleKeyRelease);

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutReshapeFunc(resize);
    // glutIdleFunc(animate);
    animate();

    glutMainLoop();

    destroy();

    return 0;
}
