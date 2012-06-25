#ifndef Scene_HH
#define Scene_HH

#include <string>
#include <vector>
#include <set>
#include <cmath>
#include <mutex>

#include <glm/glm.hpp>
#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>

#include "debug.hh"
#include "drawable.hh"
#include "material.hh"
#include "glcommon.hh"

class Scene: public Drawable {
public:
    Scene(const std::string& filename);
    ~Scene();

    void load();
    void loadVao();
    void unload();
    void unloadVao();

    virtual void draw();
    virtual bool collision(const glm::vec3& point, glm::vec3& movement) const;
    virtual void visibility(const glm::vec3& point);

private:

    class Mesh {
    public:
        Mesh();
        virtual ~Mesh() {};
        virtual void draw() const;
        bool load(const aiMesh* mesh, std::vector<GLVertex>& gl_vertexes, std::vector<GLFace>& gl_faces);
        // glm::vec3 min() const { return min_; }
        // glm::vec3 max() const { return max_; }
        unsigned int materialIndex() const { return material_; }
        unsigned int firstFace() const { return start_face_; }
        unsigned int faceCount() const { return face_count_; }

    private:
        unsigned int start_face_;
        unsigned int face_count_;
        unsigned int material_;

        // glm::vec3 min_;
        // glm::vec3 max_;
    };


    bool loaded_;
    bool ready_;
    bool to_delete_;
    std::mutex status_mutex_;

    std::string filename_;

    unsigned int vao_;
    unsigned int buffers_[2];
    std::vector<GLVertex> gl_vertexes;
    std::vector<GLFace> gl_faces;

    std::vector<Mesh*> meshes_;
    std::vector<Material*> materials_;
};

#endif
