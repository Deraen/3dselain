#include <cmath>

#include <GL/gl.h>

#include "drawable.hh"
#include "light.hh"

GLenum Light::lightN_ = GL_LIGHT0;

Light::Light():
    n(lightN_),
    position(),
    specular(),
    ambient (),
    diffuse ()
{
    ++lightN_;

    glEnable(n);
    // glLightf(n, GL_LINEAR_ATTENUATION, 0.01); // Valo heikkenee hieman etäisyyden funktiona
    // glLightf(n, GL_QUADRATIC_ATTENUATION, 0.01);
}

bool Light::collision(const Vec3& point, Vec3& movement) const {
    return false;
}

Light::~Light() {
}

void Light::setPos(float x, float y, float z, float w) {
    position[0] = x;
    position[1] = y;
    position[2] = z;
    position[3] = w;
}

void Light::setSpecular(GLfloat r, GLfloat g, GLfloat b) {
    specular[0] = r;
    specular[1] = g;
    specular[2] = b;
    specular[3] = 1.0; //w
}

void Light::setAmbient(GLfloat r, GLfloat g, GLfloat b) {
    ambient[0] = r;
    ambient[1] = g;
    ambient[2] = b;
    ambient[3] = 1.0; //w
}

void Light::setDiffuse(GLfloat r, GLfloat g, GLfloat b) {
    diffuse[0] = r;
    diffuse[1] = g;
    diffuse[2] = b;
    diffuse[3] = 1.0; //w
}

void Light::draw() const {
    glLightfv(n, GL_POSITION, position);
    glLightfv(n, GL_SPECULAR, specular);
    glLightfv(n, GL_AMBIENT, ambient);
    glLightfv(n, GL_DIFFUSE, diffuse);
}

