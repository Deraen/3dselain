#ifndef LIGHT_HH
#define LIGHT_HH

#include <GL/glew.h>

#include "drawable.hh"
#include "animated.hh"
#include "vec3.hh"

class Light : public Drawable {
public:
    Light();

    ~Light();

    void setPos(float x, float y, float z, float w = 1.0);

    void setSpecular(GLfloat r, GLfloat g, GLfloat b);

    void setAmbient(GLfloat r, GLfloat g, GLfloat b);

    void setDiffuse(GLfloat r, GLfloat g, GLfloat b);

    void draw() const;

    virtual bool collision(const Vec3& point, Vec3& movement) const;

protected:
    GLenum n;
    GLfloat position[4];
private:
    static GLenum lightN_;

    GLfloat specular[4];
    GLfloat ambient[4];
    GLfloat diffuse[4];
};

#endif
