#ifndef Scene_HH
#define Scene_HH

#include <string>
#include <vector>
#include <set>
#include <cmath>
#include <map>

// #include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>       // Output data structure

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
    glm::vec3* a;
    glm::vec3* b;
    glm::vec3* c;
    glm::vec3 normal;
    float d;
    glm::vec3 min;
    glm::vec3 max;

    Face(glm::vec3* a_, glm::vec3* b_, glm::vec3* c_);
    bool isPointInside(const glm::vec3& point) const;
};

class Mesh {
public:
    Mesh();
    virtual void draw() const;
    bool load(const aiMesh* mesh, std::vector<GLVertex>& gl_vertexes, std::vector<GLFace>& gl_faces);
    glm::vec3 min() const { return min_; }
    glm::vec3 max() const { return max_; }
    unsigned int materialIndex() const { return material_; }
    unsigned int firstFace() const { return start_face_; }
    unsigned int faceCount() const { return face_count_; }

private:
    unsigned int start_face_;
    unsigned int face_count_;
    unsigned int material_;

    glm::vec3 min_;
    glm::vec3 max_;
};

class Scene: public Drawable {
public:
    Scene(const std::string& filename);
    ~Scene();

    void load();
    void unload();

    virtual void draw() const;
    virtual bool collision(const glm::vec3& point, glm::vec3& movement) const;
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
    glm::vec3 min_;
    glm::vec3 max_;
};

#endif
