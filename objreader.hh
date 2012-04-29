#ifndef OBJREADER_HH
#define OBJREADER_HH

#include <string>
#include <vector>
#include <set>
#include <cmath>

#include <GL/gl.h>

#include "vec3.hh"
#include "debug.hh"
#include "drawable.hh"

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

// struct Vertex {
//     struct Face {
//         Vertex* a;
//         Vertex* b;
//         Vertex* c;
//         Vec3 normal;
//         Vec3 min;
//         Vec3 max;
//         Face* next;

//         Face(Vertex* a_, Vertex* b_, Vertex* c_):
//             a(a_), b(b_), c(c_),
//             normal((*b - *a).cross((*c - *a))),
//             min(std::min(a->x, std::min(b->x, c->x)),
//                 std::min(a->y, std::min(b->y, c->y)),
//                 std::min(a->z, std::min(b->z, c->z))),
//             max(std::max(a->x, std::max(b->x, c->x)),
//                 std::max(a->y, std::max(b->y, c->y)),
//                 std::max(a->z, std::max(b->z, c->z))),
//             next(NULL)
//         {}
//     };

//     float x;
//     float y;
//     float z;
//     Vertex* lesser;
//     Vertex* greater;
//     Face* faces;

//     Vertex(const float x_, const float y_, const float z_):
//         x(x_), y(y_), z(z_),
//         lesser(NULL),
//         greater(NULL),
//         faces(NULL)
//     {}
//     Face* appendFace(Vertex* a, Vertex* b, Vertex* c) {
//         Face* i = faces;
//         while (i != NULL && i->next != NULL) {
//             i = i->next;
//         }

//         Face* uusi = new Face(a, b, c);
//         if (i == NULL) faces   = uusi;
//         else           i->next = uusi;
//         return uusi;
//     }
//     Vec3 operator-(const Vertex& b) {
//         return Vec3(x - b.x,
//                     y - b.y,
//                     z - b.z);
//     }
//     float weightBetween(const Vec3& point) {
//         return pow(point.x - x, 2) + pow(point.y - y, 2) + pow(point.z - z, 2);
//     }
//     friend Debug& operator<<(Debug& debug, const Vertex& b) {
//         debug << b.x << ", " << b.y << ", " << b.z;
//         return debug;
//     }
// };
struct Face {
    Vec3* a;
    Vec3* b;
    Vec3* c;
    Vec3 normal;
    float d;
    // Vec3 min;
    // Vec3 max;
    Face* next;

    Face(Vec3* a_, Vec3* b_, Vec3* c_):
        a(a_), b(b_), c(c_),
        normal((*b - *a).cross(*c - *a)),
        d(0.0),
        // min(std::min(a->x, std::min(b->x, c->x)),
        //     std::min(a->y, std::min(b->y, c->y)),
        //     std::min(a->z, std::min(b->z, c->z))),
        // max(std::max(a->x, std::max(b->x, c->x)),
        //     std::max(a->y, std::max(b->y, c->y)),
        //     std::max(a->z, std::max(b->z, c->z))),
        next(NULL)
    {
        normal.normalize();
        d = -normal.x * a->x - normal.y * a->y - normal.z * a->z;
    }

    // http://www.blackpawn.com/texts/pointinpoly/default.html
    // Barycentric Technique

    // eli selvitetään paljonko A kulmasdta liikutaan
    // AB ja AC suuntiin. Jos jompaan kumpaan negatiivinen määrä
    // piste on AB tai AC viivan väärällä puolella.
    // jos yhteensä yli 1 ollaan BC viivan ohi.
    bool isPointInside(const Vec3& point) {
        Vec3 v0 = *c - *a;
        Vec3 v1 = *b - *a;
        Vec3 v2 = point - *a;

        float dot00 = v0.dot(v0);
        float dot01 = v0.dot(v1);
        float dot02 = v0.dot(v2);
        float dot11 = v1.dot(v1);
        float dot12 = v1.dot(v2);

        float invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
        float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

        return (u >= 0) && (v >= 0) && (u + v < 1);
    }
};

// struct VertexCompare {
//     enum SortType { X, Y, Z };

//     VertexCompare(SortType type_):
//         type(type_),
//         control(NULL)
//     {}
//     VertexCompare(SortType type_, Vertex* control_):
//         type(type_),
//         control(control_)
//     {}

//     bool operator()(const Vertex* a, const Vertex* b) {
//         if (type == X) {
//             return a->x <= b->x;
//         } else if (type == Y) {
//             return a->y <= b->y;
//         } else if (type == Z) {
//             return a->z <= b->z;
//         }
//         return false;
//     }

//     SortType type;
//     Vertex* control;
// };

class ObjReader: public Drawable {
public:
    ObjReader(const char* filename);

    virtual void draw();

    // virtual Vertex* nearestPoint(const Vec3& point);
    virtual bool collision(const Vec3& point, Vec3& movement, unsigned int depth = 0);

    static bool drawNormals_; 
    static bool drawFaceCenters_;

private:
    // void travel(Vertex* i, const Vec3& point, Vertex*& best, float& bestWeight, unsigned int depth);
    // Vertex* kdtree(unsigned int start,
    //                unsigned int stop,
    //                unsigned int depth);

    GLuint vbo_;
    GLuint vinx_;
    unsigned int faceCount_;
    // std::vector<Vertex*> vertexes_;
    std::vector<Face*> faces_;
    // Vertex* tree_;
    // Edellisen nearestPoint piste piirtämistä varten (debug)
    // Vertex* best_;
    // Bounding box
    Vec3 min_;
    Vec3 max_;
    Vec3 p;
    Vec3 q;
    Face* tormatty_;
};

#endif
