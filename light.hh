#ifndef LIGHT_HH
#define LIGHT_HH

#include <GL/gl.h>

#include "drawable.hh"
#include "animated.hh"

class Light : public Drawable {
public:
    Light(GLenum n_);

    ~Light();

    void setPos(float x, float y, float z);

    void setSpecular(GLfloat r, GLfloat g, GLfloat b);

    void setAmbient(GLfloat r, GLfloat g, GLfloat b);

    void setDiffuse(GLfloat r, GLfloat g, GLfloat b);

    void draw();

protected:
    GLenum n;
    float* position;
private:
    GLfloat* specular;
    GLfloat* ambient;
    GLfloat* diffuse;
};

#endif
