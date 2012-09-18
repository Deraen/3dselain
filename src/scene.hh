#ifndef Scene_HH
#define Scene_HH

#include <string>
#include <vector>
#include <set>
#include <cmath>
#include <mutex>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <glm/glm.hpp>
// #include <assimp/assimp.hpp>
// #include <assimp/aiScene.h>
#include <assimp/mesh.h>

#include "debug.hh"
#include "drawable.hh"
#include "glcommon.hh"
#include "material.hh"

class Scene: public Drawable {
public:
    Scene(const std::string& filename);
    ~Scene();

    void load();
    void loadVao();
    void unload();
    void unloadVao();

    void write();

    virtual void draw();
    virtual bool collision(const glm::vec3& point, glm::vec3& movement) const;
    virtual void visibility(const glm::vec3& point);


    const std::string& name() const { return filename_.native(); }

    class Mesh {
    public:
        Mesh();
        // ~Mesh() {};
        void draw() const;
        bool load(const aiMesh* mesh, std::vector<GLVertex>& gl_vertexes, std::vector<GLFace>& gl_faces, const std::string& material_name);

        void write(boost::filesystem::ofstream& file);

        unsigned int firstFace() const { return start_face_; }
        unsigned int faceCount() const { return face_count_; }

    private:
        unsigned int start_face_;
        unsigned int face_count_;
        unsigned int material_;
    };

private:

    bool loaded_;
    bool ready_;
    bool to_delete_;
    std::mutex status_mutex_;

    boost::filesystem::path filename_;

    unsigned int vao_;
    unsigned int buffers_[2];
    std::vector<GLVertex> gl_vertexes;
    std::vector<GLFace> gl_faces;

    std::vector<Mesh*> meshes_;
};

const unsigned int MESH_BYTES = sizeof(Scene::Mesh);

union MeshUnion {
    Scene::Mesh data;
    char chars[MESH_BYTES];
    MeshUnion(): data() {}
    MeshUnion(const Scene::Mesh& d): data(d) {}
};

// const unsigned int MATERIAL_BYTES = sizeof(Material);

// union MaterialUnion {
//     Material data;
//     char chars[MATERIAL_BYTES];
//     MaterialUnion(): data() {}
//     MaterialUnion(const Material& d): data(d) {}
// };

#endif
