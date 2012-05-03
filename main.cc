/*
-gldebug käynnistysvipu saattaa auttaa opengl virheiden etsinnässä.

Hiirellä voi tehdä asioita:
Vasemmalla näppäimellä liikutaan eteenpäin (kuin 'i')
Keskinäppäimellä käytetään Voimaa, joka ainakin heiluttaa kättä ja saattaa
    tehdä kameran edessä näkyville asioille jotakin.
Oikealla näppäimellä käännetään kameraa.

Vaadittujen painikkeiden lisäksi:
'<' - Wireframe päälle/pois.
'Z' - Kolmioiden normaalien piirto.
'X' - Kolmioiden keskipisteiden piirto.
'C' - Tekstuurit pois ja päälle.
'V' - Tulostaa kameran koordinaatit.
'B' - Törmäystarkistus päälle ja pois.
Space - Ylöspäin kamerasta riippumatta.

Texturit:
Kivi: http://agf81.deviantart.com/art/Stone-Texture-Seamless-197981741 - Some rights reserved. This work is licensed under a Creative Commons Attribution 3.0 License.
Kiviseinä: http://agf81.deviantart.com/gallery/31629552#/d49w3ju - Some rights reserved. This work is licensed under a Creative Commons Attribution 3.0 License.
*/

#include <iostream>
#include <vector>
using std::vector;
#include <map>
using std::map;
#include <string>
using std::string;

// Define jotain jänniä juttuja varten
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>

#include "camera.hh"
#include "drawable.hh"
#include "texture.hh"
#include "objreader.hh"
#include "solidmaterial.hh"
#include "light.hh"
#include "cube.hh"

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
unsigned int windowWidth_, windowHeight_;
int dX_, dY_, oldX_, oldY_;
bool wireframe_ = false;
bool collisionDetection_ = true;
double secondsSinceStart_ = 0.0; // hyvin epätarkka arvio (glutTimer...)
map<string, Texture*> textures_;
vector<Drawable*> objects_;
Drawable* kasi_;

// --- TEXTUURIT ---
void addTexture(const string& key, const string& filename) {
    if (textures_.find(filename) != textures_.end()) {
        Debug::start() << "Yritettiin lisätä tekstuuria samalla avaimella" << Debug::end();
        return;
    }
    textures_[key] = new Texture(filename);
}

Texture* getTexture(const string& key) {
    map<string, Texture*>::iterator i = textures_.find(key);
    if (i == textures_.end()) {
        Debug::start() << "Ei löydetty textuuria" << Debug::end();
        return NULL;
    }
    return i->second;
}

// --- OBJEKTIT ---

class Sun : public Light, public Animated {
public:
    Sun():
        Light()
    {
        setSpecular(1.0, 1.0, 1.0);
        setAmbient(0.1, 0.1, 0.1);
        setDiffuse(1.0, 1.0, 1.0);
    }

    void draw() {
        glTranslatef(position[0], position[1], position[2]);
        glutSolidSphere(2.0, 16, 16);

        Light::draw();
    }
    void animate(float time) {
        setPos(100 * sin(time / 5.0), 125.0, 100 * cos(time / 5.0));
    }
};

class Valo : public Light {
public:
    Valo(float x, float y, float z):
        Light()
    {
        setSpecular(1.0, 1.0, 1.0);
        setAmbient(0.1, 0.1, 0.1);
        setDiffuse(1.0, 1.0, 1.0);
        setPos(x, y, z);
    }

    void draw() {
        glTranslatef(position[0], position[1], position[2]);
        glutSolidSphere(1.0, 16, 16);

        Light::draw();
    }
};

class Alusta : public ObjReader {
public:
    Alusta():
        ObjReader("obj/alusta.obj"),
        material_()
    {
        material_.setShinines(100);
        material_.setDiffuse(0.75, 0.75, 0.75);
        material_.setSpecular(0.85, 0.85, 0.85);
    }
    ~Alusta() {}

    void draw() {
        material_.use();
        getTexture("stone")->start();

        ObjReader::draw();

        getTexture("stone")->end();
    }
private:
    SolidMaterial material_;
};

class Talo : public ObjReader {
public:
    Talo():
        ObjReader("obj/talo.obj"),
        material_()
    {
        material_.setShinines(100);
        material_.setDiffuse(0.65, 0.85, 0.65);
        material_.setSpecular(0.75, 0.95, 0.75);
    }
    ~Talo() {}

    void draw() {
        getTexture("stonewall")->start();
        material_.use();

        ObjReader::draw();
        getTexture("stonewall")->end();
    }
private:
    SolidMaterial material_;
};

/*
Lataa mallin moneen kertaan.
Voisi kiertää tallentamalla objreaderin staticiksi mutta olkoon
*/
class Muurit : public ObjReader {
public:
    Muurit(unsigned int angle):
        ObjReader("obj/muurit.obj"),
        angle_(angle),
        material_()
    {
        material_.setShinines(100);
        material_.setDiffuse(0.75, 0.75, 0.75);
        material_.setSpecular(0.85, 0.85, 0.85);
    }
    ~Muurit() {}

    void draw() {
        material_.use();
        glRotatef(angle_, 0.0, 1.0, 0.0);
        getTexture("stonewall")->start();

        ObjReader::draw();

        getTexture("stonewall")->end();
    }
private:
    unsigned int angle_;
    SolidMaterial material_;
};

class Kasi : public ObjReader {
public:
    Kasi():
        ObjReader("obj/kasi.obj"),
        material_()
    {
        material_.setShinines(100);
        material_.setDiffuse(0.6, 0.3, 0.4);
        material_.setSpecular(0.6, 0.3, 0.4);
    }
    ~Kasi() {}

    bool collision(const Vec3& point, Vec3& movement) {
        return false;
    }

    void draw() {
        material_.use();
        // hyödynnetään globaaleja rumasti...
        if (buttons_[GLUT_MIDDLE_BUTTON]) {
            glRotatef(5 * sin(secondsSinceStart_ * 3), 1.0, 0.0, 0.0);
        }

        ObjReader::draw();
    }
private:
    SolidMaterial material_;
};

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

    addTexture("stone", "tex/stone.tga");
    addTexture("stonewall", "tex/stonewall.tga");
    kasi_ = new Kasi;
    objects_.push_back(new Sun);
    objects_.push_back(new Alusta);
    objects_.push_back(new Talo);
    objects_.push_back(new Valo(71.0, 92.0, -155.0));
    objects_.push_back(new Valo(203.0, 87.0, -144.0));
    objects_.push_back(new Valo(142.0, 90.0, -79.0));
    objects_.push_back(new Muurit(0));
    objects_.push_back(new Muurit(90));
    objects_.push_back(new Muurit(180));
    objects_.push_back(new Muurit(270));
    objects_.push_back(new Cube(1.0, 0.5, 0.5, 20, 0, 0));
    objects_.push_back(new Cube(0.5, 1.0, 0.5, 0, 20, 0));
    objects_.push_back(new Cube(0.5, 0.5, 1.0, 0, 0, 20));
    objects_.push_back(new Cube(0.5, 0.5, 1.0, 205, 33, -143));
    objects_.push_back(new Cube(0.5, 0.5, 1.0, 177, 36, -76));
}

void destroy() {
    for (map<string, Texture*>::iterator i = textures_.begin();
         i != textures_.end(); ++i) {
        delete i->second;
    }

    for (unsigned int i = 0; i < objects_.size(); ++i) {
        delete objects_.at(i);
    }
    delete kasi_;
}

void handleKey(unsigned char key, int, int) {
    if (key == '<') wireframe_ = !wireframe_;
    if (key == 'z') ObjReader::drawNormals_ = !ObjReader::drawNormals_;
    if (key == 'x') ObjReader::drawFaceCenters_ = !ObjReader::drawFaceCenters_;
    if (key == 'c') Texture::enabled = !Texture::enabled;
    if (key == 'v') Debug::start()[1] << "Kameran koordinaatti " << camera_.getPos() << Debug::end();
    if (key == 'b') collisionDetection_ = !collisionDetection_;
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
    gluPerspective(45.0, static_cast<float>(windowWidth_) / static_cast<float>(windowHeight_), 0.01, 1000);

    // nopea patentti jolla käsi piirretään sojottamaan ulos näytöstä
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    kasi_->draw();

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

    // Alla olevat siirrot tallentuvat kameralle
    if (keys_['i'] || buttons_[GLUT_LEFT_BUTTON]) camera_.move(-1.0);
    if (keys_['k']) camera_.move(1);
    if (keys_['j']) camera_.strafe(-1);
    if (keys_['l']) camera_.strafe(1);
    if (keys_['y']) camera_.moveHeight(1);
    if (keys_['h']) camera_.moveHeight(-1);
    // Yhteenlasketut siirrot
    Vec3 movement = camera_.getMovement();

    if (keys_[' ']) movement += Vec3(0.0, 1.0, 0.0);
    // Ollaanko törmäämässä johonkin?
    // törmäys päivittää liike vektoria törmäyksen mukaisesti
    for (unsigned int i = 0; i < objects_.size() && collisionDetection_; ++i) {
        objects_.at(i)->collision(camera_.getPos(), movement);
    }

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
        // 3 astetta
        double heading = -dX_ / w * 3.0;
        double pitch   = -dY_ / h * 3.0;
        camera_.heading(heading);
        camera_.pitch(pitch);
        glutWarpPointer(windowWidth_ / 2, windowHeight_ / 2);
    }
}

void mouse(int button, int state, int x, int y) {
    if (button < GLUT_LEFT_BUTTON
     || button > GLUT_RIGHT_BUTTON) return;

    if (state == GLUT_DOWN) {
        buttons_[button] = true;
        if (button == GLUT_RIGHT_BUTTON) {
            // talletaan hiiren sijainti
            oldX_ = x;
            oldY_ = y;
            dX_ = 0;
            dY_ = 0;
            // hiiri keskelle jotta helpompi pyörittää
            glutWarpPointer(windowWidth_ / 2, windowHeight_ / 2);
        }
    } else if (state == GLUT_UP) {
        buttons_[button] = false;
        if (button == GLUT_RIGHT_BUTTON) {
            // palautetaa hiiri vanhaan paikkaan
            glutWarpPointer(oldX_, oldY_);
        }
    }
}

void motion(int x, int y) {
    if (buttons_[GLUT_RIGHT_BUTTON]) {
        // talletetaan koordinaatit jotta voidaan kääntää kameraa
        // kun ensikerran piirretään ruutua
        double w = static_cast<double>(windowWidth_) / 2;
        double h = static_cast<double>(windowHeight_) / 2;
        dX_ = (x - w);
        dY_ = (y - h);
    }
}

void animate(int a = 0) {
    handleKeys();
    glutPostRedisplay();

    secondsSinceStart_ += static_cast<double>(MS_PER_FRAME) / pow(10, 3);

    // pidetään esim. nois 60fps piirtämällä ruutuun vain esim. 15ms välein
    glutTimerFunc(MS_PER_FRAME, animate, 0);
}

int main(int argc, char *argv[]) {
    // Alustetan GLUT ja luodaan ikkuna
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(DEF_WINDOW_WIDTH, DEF_WINDOW_HEIGHT);
    glutCreateWindow("...GL");

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
