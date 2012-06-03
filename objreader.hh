#ifndef OBJREADER_HH
#define OBJREADER_HH

#include <string>
#include <vector>
#include <set>
#include <cmath>
#include <map>

#include <GL/gl.h>

#include "vec3.hh"
#include "debug.hh"
#include "drawable.hh"

struct Vec4 {
    float r, g, b, a;

    Vec4(float r_, float g_, float b_, float a_):
        r(r_), g(g_), b(b_), a(a_)
    {}
};


#pragma pack(1)
struct GLVertex {
    double x, y, z;
    float nx, ny, nz;
    float u, v;
    float r, g, b, a;

    GLVertex(double x_, double y_, double z_):
        x(x_), y(y_), z(z_),
        nx(0.0), ny(0.0), nz(0.0),
        u(0.0), v(0.0),
        r(0.0), g(0.0), b(0.0), a(0.0)
    {}

    void setColor(Vec4 color) {
        r = color.r;
        g = color.g;
        b = color.b;
        a = color.a;
    }
};

struct GLFace {
    unsigned int a;
    unsigned int b;
    unsigned int c;

    GLFace(unsigned int a_, unsigned int b_, unsigned int c_):
        a(a_), b(b_), c(c_)
    {}
};
#pragma pack()

struct Face {
    Vec3* a;
    Vec3* b;
    Vec3* c;
    Vec3 normal;
    double d;
    Vec3 min;
    Vec3 max;
    // Face* next;

    Face(Vec3* a_, Vec3* b_, Vec3* c_):
        a(a_), b(b_), c(c_),
        normal((*b - *a).cross(*c - *a)),
        d(0.0),
        min(std::min(a->x, std::min(b->x, c->x)) - CAMERA_R,
            std::min(a->y, std::min(b->y, c->y)) - CAMERA_R,
            std::min(a->z, std::min(b->z, c->z)) - CAMERA_R),
        max(std::max(a->x, std::max(b->x, c->x)) + CAMERA_R,
            std::max(a->y, std::max(b->y, c->y)) + CAMERA_R,
            std::max(a->z, std::max(b->z, c->z)) + CAMERA_R)
        // next(NULL)
    {
        normal.normalize();
        d = -normal.x * a->x - normal.y * a->y - normal.z * a->z;
    }

    // http://www.blackpawn.com/texts/pointinpoly/default.html
    // Barycentric Technique

    // eli selvitetään paljonko A kulmasta liikutaan
    // AB ja AC suuntiin. Jos jompaan kumpaan negatiivinen määrä
    // piste on AB tai AC viivan väärällä puolella.
    // jos yhteensä yli 1 ollaan BC viivan ohi.
    bool isPointInside(const Vec3& point) {
        Vec3 v0 = *c - *a;
        Vec3 v1 = *b - *a;
        Vec3 v2 = point - *a;

        double dot00 = v0.dot(v0);
        double dot01 = v0.dot(v1);
        double dot02 = v0.dot(v2);
        double dot11 = v1.dot(v1);
        double dot12 = v1.dot(v2);

        double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
        double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

        return (u >= 0) && (v >= 0) && (u + v < 1);
    }
};

class ObjReader: public Drawable {
public:
    ObjReader(const std::string& dir, const std::string& filename);
    ~ObjReader();

    virtual void draw();
    virtual bool collision(const Vec3& point, Vec3& movement);
    virtual bool rayCollision(const Vec3& point, const Vec3& ray, float& distance);

    static bool drawNormals_;
    static bool drawFaceCenters_;

private:

    void readMaterials(const std::string& dir, const std::string& filename);

    GLuint vbo_;
    GLuint vinx_;
    std::vector<Face*> faces_;
    Vec3 min_;
    Vec3 max_;

    std::map<std::string, Vec4*> colors_;
};

#endif
