#ifndef OBJREADER_HH
#define OBJREADER_HH

#include <string>
#include <vector>
#include <set>

#include <GL/gl.h>

#include "vec3.hh"
#include "debug.hh"
#include "drawable.hh"

struct Vertex {
    struct Face {
        Vertex* a;
        Vertex* b;
        Vertex* c;
        Vec3 normal;
        Face* next;

        Face(Vertex* a_, Vertex* b_, Vertex* c_):
            a(a_), b(b_), c(c_),
            normal((*b - *a) * (*c - *a)),
            next(NULL)
        {}
    };

    float x;
    float y;
    float z;
    Vertex* lesser;
    Vertex* greater;
    Face* faces;

    Vertex(const float x_, const float y_, const float z_):
        x(x_), y(y_), z(z_),
        lesser(NULL),
        greater(NULL),
        faces(NULL)
    {}
    Face* appendFace(Vertex* a, Vertex* b, Vertex* c) {
        Face* i = faces;
        while (i != NULL && i->next != NULL) {
            i = i->next;
        }

        Face* uusi = new Face(a, b, c);
        if (i == NULL) faces   = uusi;
        else           i->next = uusi;
        return uusi;
    }
    Vec3 operator-(const Vertex& b) {
        return Vec3(x - b.x,
                    y - b.y,
                    z - b.z);
    }
    float weightBetween(const Vec3& point) {
        return pow(point.x - x, 2) + pow(point.y - y, 2) + pow(point.z - z, 2);
    }
};

struct VertexCompare {
    enum SortType { X, Y, Z };

    VertexCompare(SortType type_):
        type(type_),
        control(NULL)
    {}
    VertexCompare(SortType type_, Vertex* control_):
        type(type_),
        control(control_)
    {}

    bool operator()(const Vertex* a, const Vertex* b) {
        if (type == X) {
            return a->x <= b->x;
        } else if (type == Y) {
            return a->y <= b->y;
        } else if (type == Z) {
            return a->z <= b->z;
        }
        return false;
    }

    SortType type;
    Vertex* control;
};

class ObjReader: public Drawable {
public:
    ObjReader(const char* filename);

    virtual void draw();

    virtual Vertex* nearestPoint(const Vec3& point);
    virtual bool collision(const Vec3& point, Vec3& movement);

private:
    void travel(Vertex* i, const Vec3& point, Vertex*& best, float& bestWeight, unsigned int depth);
    Vertex* kdtree(unsigned int start,
                   unsigned int stop,
                   unsigned int depth);

    GLuint vbo_;
    GLuint vinx_;
    unsigned int faceCount_;
    std::vector<Vertex*> vertexes_;
    Vertex* tree_;
    // Edellisen nearestPoint piste piirtämistä varten (debug)
    Vertex* best_;
    // Bounding box
    Vec3 min_;
    Vec3 max_;

    struct GLVertex {
        float x, y, z;
        float nx, ny, nz;

        GLVertex(float x_, float y_, float z_):
            x(x_), y(y_), z(z_),
            nx(0.0), ny(0.0), nz(0.0)
        {}
    };

    struct GLFace {
        unsigned int a;
        unsigned int b;
        unsigned int c;

        GLFace(unsigned int a_, unsigned int b_, unsigned int c_):
            a(a_), b(b_), c(c_)
        {}
    };


        std::vector<GLVertex> glVertexes;
        std::vector<GLFace> glFaces;
};

#endif
