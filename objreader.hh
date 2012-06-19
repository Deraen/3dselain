#ifndef Scene_HH
#define Scene_HH

#include <string>
#include <vector>
#include <set>
#include <cmath>
#include <map>

// #include <GL/glew.h>

#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>       // Output data structure

#include "vec3.hh"
#include "debug.hh"
#include "drawable.hh"
#include "material.hh"

#pragma pack(1)
struct GLVertex {
    float x, y, z;
    float nx, ny, nz;
    // 6 * 4 bytes = 24

    // float u, v;

    GLVertex(float x_, float y_, float z_):
        x(x_), y(y_), z(z_),
        nx(0.0), ny(0.0), nz(0.0)//,
        // u(0.0), v(0.0)
    {}

    void setNormal(float x_, float y_, float z_) {
        nx = x_; ny = y_; nz = z_;
    }

    void setTexture(float u_, float v_) {
        // u = u_; v = v_;
    }
};
#pragma pack()

#pragma pack(1)
struct GLFace {
    unsigned int a;
    unsigned int b;
    unsigned int c;

    // 3 * 2 bytes = 6... fuu
    // char padding[2];

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
    float d;
    Vec3 min;
    Vec3 max;

    Face(Vec3* a_, Vec3* b_, Vec3* c_);
    bool isPointInside(const Vec3& point) const;
};

class Mesh {
public:
    Mesh();
    virtual void draw() const;
    bool load(const aiMesh* mesh, std::vector<GLVertex>& gl_vertexes, std::vector<GLFace>& gl_faces);
    Vec3 min() const { return min_; }
    Vec3 max() const { return max_; }
    unsigned int materialIndex() const { return material_; }
    unsigned int firstFace() const { return start_face_; }
    unsigned int faceCount() const { return face_count_; }

private:
    unsigned int start_face_;
    unsigned int face_count_;
    unsigned int material_;

    Vec3 min_;
    Vec3 max_;
};

class Scene: public Drawable {
public:
    Scene(const std::string& filename);
    ~Scene();

    void load();
    void unload();

    virtual void draw() const;
    virtual bool collision(const Vec3& point, Vec3& movement) const;
    // virtual bool rayCollision(const Vec3& point, const Vec3& ray, float& distance) const;

    static bool draw_normals_;
    static bool draw_face_centers_;

private:
    bool loaded_;
    std::string filename_;

    GLuint vao_;
    // GLuint vertices_buffer_;
    // GLuint color_buffer_;
    // GLuint vinx_;
    std::vector<Face*> faces_;
    std::vector<Mesh*> meshes_;
    std::vector<Material*> materials_;
    Vec3 min_;
    Vec3 max_;
};

#endif
