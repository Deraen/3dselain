#include <iostream>
#include <vector>
using std::vector;
#include <map>
using std::map;
#include <string>
using std::string;

#include <GL/glew.h>
// Define jotain jänniä juttuja varten
// #define GL_GLEXT_PROTOTYPES
// #include <GL/gl.h>
#include <GL/glfw.h>

#include "camera.hh"
#include "drawable.hh"
#include "texture.hh"
#include "objreader.hh"
#include "animated.hh"
#include "shader.hh"


#include <stdio.h>


// --- VAKIOT ---
const int DEF_WINDOW_WIDTH = 800;
const int DEF_WINDOW_HEIGHT = 600;
const unsigned int FPS = 60;
const unsigned int MS_PER_FRAME = pow(10, 3) / FPS;
const unsigned int NUM_OF_KEYS = sizeof(char);
const unsigned int NUM_OF_BUTTONS = 3;

// --- GLOBAALIT ---
// Camera camera_(6600.0, 20.0, -4800.0);
Camera camera_(0.0, 0.0, 0.0);
int dX_, dY_, oldX_, oldY_;
bool wireframe_ = false;
bool collisionDetection_ = true;
map<string, Texture*> textures_;
map<string, Shader*> shaders_;
vector<Drawable*> objects_;

namespace {
    float divide(float a, float b) {
        return (a + 1 / b) / b;
    }
}

// --- TEXTUURIT ---
void addTexture(const string& key, const string& filename) {
    if (textures_.find(key) != textures_.end()) {
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

// --- SHADERIT ---

void addShader(const string& key) {
    if (shaders_.find(key) != shaders_.end()) {
        Debug::start() << "Yritettiin lisätä tekstuuria samalla avaimella" << Debug::end();
        return;
    }
    shaders_[key] = new Shader;
}

Shader* getShader(const string& key) {
    map<string, Shader*>::iterator i = shaders_.find(key);
    if (i == shaders_.end()) {
        Debug::start() << "Ei löydetty shaderia" << Debug::end();
        return NULL;
    }
    return i->second;
}

void init() {
    glClearColor(0.0, 0.0, 0.2, 0.0); // Ruudun tyhjennysväri
    // glEnable(GL_DEPTH_TEST);  // Z-testi

    // glEnable(GL_CULL_FACE); // Ei piirretä kolmioita kameran takana?
    // glCullFace(GL_FRONT);
    // glFrontFace(GL_CW);

    glEnable(GL_MULTISAMPLE);

    // glEnable(GL_LIGHTING);
    // glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);  // Sävytys: GL_FLAT / GL_SMOOTH

    addShader("lightning");
    getShader("lightning")->addVertexShader("shaders/lightning.vert");
    getShader("lightning")->addFragmentShader("shaders/lightning.frag");
    getShader("lightning")->load();
    getShader("lightning")->begin();
    // getShader("lightning")->bindAttrib(0, "in_position");
    // getShader("lightning")->bindAttrib(1, "in_normal");

    // addTexture("stone", "tex/stone.tga");
    // addTexture("stonewall", "tex/stonewall.tga");
    objects_.push_back(new Scene("obj/senaatintori-sim-malli/bl-001-001.obj"));
    objects_.push_back(new Scene("obj/senaatintori-sim-malli/bl-001-002.obj"));
    objects_.push_back(new Scene("obj/senaatintori-sim-malli/bl-001-004.obj"));
    objects_.push_back(new Scene("obj/senaatintori-sim-malli/bl-001-029.obj"));
    objects_.push_back(new Scene("obj/senaatintori-sim-malli/bl-001-030.obj"));
    objects_.push_back(new Scene("obj/senaatintori-sim-malli/bl-001-031.obj"));
    objects_.push_back(new Scene("obj/senaatintori-sim-malli/bl-002-027.obj"));
    objects_.push_back(new Scene("obj/senaatintori-sim-malli/bl-002-028.obj"));
    objects_.push_back(new Scene("obj/senaatintori-sim-malli/bl-002-032.obj"));
    objects_.push_back(new Scene("obj/senaatintori-sim-malli/pa-001-g100_001.obj"));
    objects_.push_back(new Scene("obj/senaatintori-sim-malli/st-keskusta.obj"));

    for (unsigned int i = 0; i < objects_.size(); ++i) {
        // XXX: fuuuu
        Scene* scene = dynamic_cast<Scene*>(objects_.at(i));
        if (scene) {
            scene->load();
        }
    }
}

void destroy() {
    for (map<string, Texture*>::iterator i = textures_.begin();
         i != textures_.end(); ++i) {
        delete i->second;
    }

    for (map<string, Shader*>::iterator i = shaders_.begin();
         i != shaders_.end(); ++i) {
        delete i->second;
    }

    for (unsigned int i = 0; i < objects_.size(); ++i) {
        delete objects_.at(i);
    }
}

void handleKey(int key, int action) {
    if (action == GLFW_PRESS) {
        if (key == '1') wireframe_ = !wireframe_;
        if (key == '2') Scene::draw_normals_ = !Scene::draw_normals_;
        if (key == '3') Scene::draw_face_centers_ = !Scene::draw_face_centers_;
        if (key == '4') Texture::enabled = !Texture::enabled;
        if (key == '5') Debug::start()[1] << "Kameran koordinaatti " << camera_.getPos() << Debug::end();
        if (key == '6') collisionDetection_ = !collisionDetection_;
    }
}

void display() {
    // Tyhjennetään ruutu ja Z-puskuri
    glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);

    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();

    int w, h;
    glfwGetWindowSize(&w, &h);

    // glFrustum(-fW, fW, -fH, fH, 1, 500);

    // nopea patentti jolla käsi piirretään sojottamaan ulos näytöstä
    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();

    // camera_.set();

    GLenum shader_handle = getShader("lightning")->handle();
    GLuint modelview_loc = glGetUniformLocation(shader_handle, "modelview");
    GLuint projection_loc = glGetUniformLocation(shader_handle, "projection");
    GLuint location_loc = glGetUniformLocation(shader_handle, "location_Matrix");
    glUniformMatrix4fv(modelview_loc, 1, GL_FALSE, camera_.projection(w, h));
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, camera_.modelview());
    glUniformMatrix4fv(location_loc, 1, GL_FALSE, camera_.location());

    if (wireframe_) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (unsigned int i = 0; i < objects_.size(); ++i) {
        // glPushMatrix();

        Drawable* obj = objects_.at(i);

        // Jos piirrettävä toteuttaa Animated luokan niin kutsutaan sen
        // animate funktiota joka esim. siirtää sen sijaintia
        Animated* animate = dynamic_cast<Animated*>(obj);
        if (animate != NULL) animate->animate(glfwGetTime());

        obj->draw();

        // glPopMatrix();
    }

    // Vaihdetaan piirtopuskuri ja näkyvä puskuri
    glfwSwapBuffers();
}

void handleKeys() {
    // Alla olevat siirrot tallentuvat kameralle
    if (glfwGetKey('W') || glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) camera_.move(-1);
    if (glfwGetKey('S')) camera_.move(1);
    if (glfwGetKey('A')) camera_.strafe(-1);
    if (glfwGetKey('D')) camera_.strafe(1);
    // Yhteenlasketut siirrot
    Vec3 movement = camera_.getMovement();

    if (glfwGetKey(' ')) movement += Vec3(0.0, 1.0, 0.0);
    // Ollaanko törmäämässä johonkin?
    // törmäys päivittää liike vektoria törmäyksen mukaisesti
    for (unsigned int i = 0; i < objects_.size() && collisionDetection_; ++i) {
        objects_.at(i)->collision(camera_.getPos(), movement);
    }

    // Suoritetaan muutettu siirto
    camera_.applyMovement(movement);

    if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
        int x, y;
        glfwGetWindowSize(&x, &y);
        // 3 astetta
        double heading = -dX_ / divide(x, 2) * 7.0;
        double pitch   = -dY_ / divide(y, 2) * 7.0;
        dX_ = 0;
        dY_ = 0;
        camera_.heading(heading);
        camera_.pitch(pitch);
        glfwGetWindowSize(&x, &y);
        glfwSetMousePos(x / 2, y / 2);
    }
}

void mouse(int key, int action) {
    if (key == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            // talletaan hiiren sijainti
            int x, y;
            glfwGetMousePos(&x, &y);
            oldX_ = x;
            oldY_ = y;
            dX_ = 0;
            dY_ = 0;
            glfwGetWindowSize(&x, &y);
            glfwSetMousePos(divide(x, 2), divide(y, 2));
            // glfwDisable( GLFW_MOUSE_CURSOR );
        } else if (action == GLFW_RELEASE) {
            glfwSetMousePos(oldX_, oldY_);
            // glfwEnable( GLFW_MOUSE_CURSOR );
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

void animate() {
    handleKeys();
    display();
}

void resize(int w, int h) {
    glViewport(0, 0, w, h);
}

int main(int argc, char *argv[]) {
    int running = GL_TRUE;

    glfwInit();
    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
    glfwOpenWindow(DEF_WINDOW_WIDTH, DEF_WINDOW_HEIGHT, 8, 8, 8, 8, 8, 0, GLFW_WINDOW);
    glfwSetWindowTitle("JAA");

    glewInit();

    init();

    glfwSetKeyCallback(handleKey);

    glfwSetMouseButtonCallback(mouse);
    glfwSetMousePosCallback(motion);

    glfwSetWindowSizeCallback(resize);

    while (running) {
        animate();

        running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
    }

    Debug::start()[1] << "Ikkuna suljettu tai ESC." << Debug::end();

    destroy();
    glfwTerminate();

    return 0;
}
