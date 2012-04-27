#include <cmath>

#include <GL/gl.h>

#include "drawable.hh"
#include "light.hh"

Light::Light(GLenum n_):
    n(n_),
    position(NULL),
    specular(NULL),
    ambient(NULL),
    diffuse(NULL)
{
    glEnable(n);

    // glLightf(n, GL_LINEAR_ATTENUATION, 0.01); // Valo heikkenee hieman etäisyyden funktiona
    // glLightf(n, GL_QUADRATIC_ATTENUATION, 0.01);
}

bool Light::collision(const Vec3& point, Vec3& movement) {
    return false;
}

Light::~Light() {
    if (position != NULL) delete[] position;
    if (specular != NULL) delete[] specular;
    if (ambient != NULL)  delete[] ambient;
    if (diffuse != NULL)  delete[] diffuse;
}

void Light::setPos(float x, float y, float z) {
    if (position == NULL) position = new float[4];
    position[0] = x;
    position[1] = y;
    position[2] = z;
    position[3] = 1.0; //w
}

void Light::setSpecular(GLfloat r, GLfloat g, GLfloat b) {
    if (specular == NULL) specular = new GLfloat[4];
    specular[0] = r;
    specular[1] = g;
    specular[2] = b;
    specular[3] = 1.0; //w
}

void Light::setAmbient(GLfloat r, GLfloat g, GLfloat b) {
    if (ambient == NULL) ambient = new GLfloat[4];
    ambient[0] = r;
    ambient[1] = g;
    ambient[2] = b;
    ambient[3] = 1.0; //w
}

void Light::setDiffuse(GLfloat r, GLfloat g, GLfloat b) {
    if (diffuse == NULL) diffuse = new GLfloat[4];
    diffuse[0] = r;
    diffuse[1] = g;
    diffuse[2] = b;
    diffuse[3] = 1.0; //w
}

void Light::draw() {
    if (position != NULL) glLightfv(n, GL_POSITION, position);
    if (specular != NULL) glLightfv(n, GL_SPECULAR, specular);
    if (ambient != NULL)  glLightfv(n, GL_AMBIENT, ambient);
    if (diffuse != NULL)  glLightfv(n, GL_DIFFUSE, diffuse);
}

