#include <string>
using std::string;
#include <vector>
using std::vector;

#include <GL3/gl3w.h>

#include <boost/filesystem.hpp>
//#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/assimp.hpp>
#include <assimp/aiPostProcess.h> // Post processing flags

#include "scene.hh"
#include "debug.hh"
#include "common.hh"
#include "gldata.hh"

#include "manager.hh"

namespace io = boost::iostreams;

// ---------- MESH --------------------

namespace {
    inline glm::vec3 color2vec(const aiColor3D& c) {
        return glm::vec3(c.r, c.g, c.b);
    }
}

Scene::Mesh::Mesh():
    start_face_(0),
    face_count_(0),
    material_(0)//,
{}

bool Scene::Mesh::load(const aiMesh* mesh, std::vector<GLVertex>& gl_vertexes, std::vector<GLFace>& gl_faces) {
    if (!mesh->HasNormals() || !mesh->HasPositions() || !mesh->HasFaces()) {
        Debug::start()[3] << "Meshiä \"" << mesh->mName.data << "\" ei voitu lukea koska se ei sisällä [normaaleita|sijainteja|pintoja]" << Debug::end();
        return false;
    }

    // bool first = true;
    unsigned int first_vertex = gl_vertexes.size();
    for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
        const aiVector3D vertex = mesh->mVertices[j];
        const aiVector3D normal = mesh->mNormals[j];

        GLVertex gl_vertex(vertex.x, vertex.y, vertex.z);
        gl_vertex.setNormal(normal.x, normal.y, normal.z);

        // if (first || vertex.x < min_.x) min_.x = vertex.x;
        // if (first || vertex.x > max_.x) max_.x = vertex.x;
        // if (first || vertex.y < min_.y) min_.y = vertex.y;
        // if (first || vertex.y > max_.y) max_.y = vertex.y;
        // if (first || vertex.z < min_.z) min_.z = vertex.z;
        // if (first || vertex.z > max_.z) max_.z = vertex.z;
        // first = false;

        // XXX: hassusti noi tuolla aiMeshissä. tarvis tutkia enempi.
        // XXX: 3ulotteiset tex coord
        // if (mesh->HasTextureCoords(j)) {
        //     const aiVector3D * const texture = mesh->mTextureCoords[j];
        //     if (mesh->mNumUVComponents[j] == 2) {
        //         gl_vertex.setTexture(texture->x, texture->y);
        //     }
        // }

        gl_vertexes.push_back(gl_vertex);
    }

    material_ = mesh->mMaterialIndex;
    start_face_ = gl_faces.size();
    for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
        const aiFace face = mesh->mFaces[j];

        if (face.mNumIndices == 3) {
            GLFace gl_face(first_vertex + face.mIndices[0],
                           first_vertex + face.mIndices[1],
                           first_vertex + face.mIndices[2]);
            gl_faces.push_back(gl_face);
            ++face_count_;
        }
    }

    return true;
}

void Scene::Mesh::draw() const {
    glDrawElements(GL_TRIANGLES, 3 * face_count_, GL_UNSIGNED_INT, (char*)NULL + start_face_ * sizeof(GLFace));
}


// --------- SCENE --------------------

Scene::Scene(const string& filename):
    loaded_(false),
    ready_(false),
    to_delete_(false),
    filename_(filename),
    vao_(0),
    // vinx_(0),
    meshes_()
{
}

// Loader-thread
void Scene::load() {
    std::unique_lock<std::mutex> lock(status_mutex_);
    if (to_delete_) return;
    if (loaded_) return;

    to_delete_ = false;
    ready_ = false;
    lock.unlock();

    if (filename_.extension() == ".gldata") {

        io::filtering_istream infile;
        infile.push(io::basic_zlib_decompressor<>());
        infile.push(io::basic_file_source<char>(filename_.native(), std::ios_base::binary));

        GlDataHeaderUnion data;
        infile.read(data.chars, GLDATAHEADER_BYTES);

        materials_.reserve(data.data.materials);
        for (unsigned int i = 0; i < data.data.materials; ++i) {
            MaterialUnion d;
            infile.read(d.chars, MATERIAL_BYTES);
            materials_.push_back(d.data);
        }

        meshes_.reserve(data.data.meshes);
        for (unsigned int i = 0; i < data.data.meshes; ++i) {
            MeshUnion d;
            infile.read(d.chars, MESH_BYTES);
            meshes_.push_back(new Mesh(d.data));
        }

        gl_vertexes.reserve(data.data.vertexes);
        for (unsigned int i = 0; i < data.data.vertexes; ++i) {
            GLVertexUnion d;
            infile.read(d.chars, VERTEX_BYTES);
            gl_vertexes.push_back(d.data);
        }

        gl_faces.reserve(data.data.faces);
        for (unsigned int i = 0; i < data.data.faces; ++i) {
            GLFaceUnion d;
            infile.read(d.chars, FACE_BYTES);
            gl_faces.push_back(d.data);
        }

    } else {

        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(filename_.c_str(),
            aiProcess_ImproveCacheLocality |
            aiProcess_RemoveRedundantMaterials |
            aiProcess_OptimizeMeshes |
            aiProcess_OptimizeGraph
        );

        if (!scene) {
            Debug::start()[0] << importer.GetErrorString() << Debug::end();
            return;
        }

        if (!scene->HasMeshes()) {
            return;
        }

        for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
            Material material;

            const aiMaterial* mat = scene->mMaterials[i];

            aiColor3D color(0.0, 0.0, 0.0);
            mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            // material->setDiffuse(color.r, color.g, color.b);
            material.diffuse = color2vec(color);

            mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
            // material->setAmbient(color.r, color.g, color.b);
            material.ambient = color2vec(color);

            // mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
            // material->setSpecular(color.r, color.g, color.b);

            float value;
            mat->Get(AI_MATKEY_OPACITY, value);
            // material->setOpacity(value);
            material.opacity = value;

            // mat->Get(AI_MATKEY_COLOR_EMISSIVE, color);
            // material->setEmission(color.r, color.g, color.b);

            // float shininess = 0.0;
            // mat->Get(AI_MATKEY_SHININESS_STRENGTH, shininess);
            // material->setShinines(shininess);

            materials_.push_back(material);
        }

        // bool first = true;
        for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
            Mesh* mesh = new Mesh;
            if (mesh->load(scene->mMeshes[i], gl_vertexes, gl_faces)) {

                // if (first || mesh->min().x < min_.x) min_.x = mesh->min().x;
                // if (first || mesh->max().x > max_.x) max_.x = mesh->max().x;
                // if (first || mesh->min().y < min_.y) min_.y = mesh->min().y;
                // if (first || mesh->max().y > max_.y) max_.y = mesh->max().y;
                // if (first || mesh->min().z < min_.z) min_.z = mesh->min().z;
                // if (first || mesh->max().z > max_.z) max_.z = mesh->max().z;
                // first = false;

                meshes_.push_back(mesh);
            } else {
                delete mesh;
            }
        }
    }

    // Rakennetaan luetuista vertexeistä ja pinnoista omia vertexeitä ja pintoja
    // joilla vähän enempi ominaisuuksia. (törmäystarkistusta varten)
    // vanhat ovat turhia tämän jälkeen (luettu jo vbo muistiin joten ei tarvita alkup
    // tietoja piirtämiseen)
    // vector<glm::vec3*> vertexes;
    // for (auto vertex: gl_vertexes) {
    //     vertexes.push_back(new glm::vec3(vertex.x, vertex.y, vertex.z));
    // }

    // for (auto face: gl_faces) {
    //     glm::vec3* a = vertexes.at(face.a);
    //     glm::vec3* b = vertexes.at(face.b);
    //     glm::vec3* c = vertexes.at(face.c);
    //     faces_.push_back(new Face(a, b, c));
    // }

    lock.lock();
    loaded_ = true;
    lock.unlock();

    Debug::start()[1] << "+++ " << filename_ << ". "
                      << materials_.size() << " Materials. "
                      << meshes_.size() << " Meshes. "
                      << gl_vertexes.size() << " Vertexes. "
                      << gl_faces.size() << " Faces." << Debug::end();
}

void Scene::visibility(const glm::vec3& point) {
    load();
}

// Main-thead
void Scene::loadVao() {
    std::unique_lock<std::mutex> lock(status_mutex_);
    if (!loaded_) return; // XXX: ensiksi pitää lataa tiedosto
    if (ready_) return;
    lock.unlock();

    glGenVertexArrays(1, &vao_);
    glGenBuffers(2, buffers_);

    glBindVertexArray(vao_);

    Shader* shader = Manager::instance().getShader("lightning");

    // in_position
    glBindBuffer(GL_ARRAY_BUFFER, buffers_[0]);
    glBufferData(GL_ARRAY_BUFFER, gl_vertexes.size() * sizeof(GLVertex), &gl_vertexes.front(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(shader->attribLoc("in_position"));
    glVertexAttribPointer(shader->attribLoc("in_position"), 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), 0);

    // in normal
    glEnableVertexAttribArray(shader->attribLoc("in_normal"));
    glVertexAttribPointer(shader->attribLoc("in_normal"), 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (char*)0 + 3 * sizeof(float));

    gl_vertexes.clear();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gl_faces.size() * sizeof(GLFace), &gl_faces.front(), GL_STATIC_DRAW);
    gl_faces.clear();

    glBindVertexArray(0);

    lock.lock();
    ready_ = true;
    lock.unlock();
}

// Loader-thread
void Scene::unload() {
    std::unique_lock<std::mutex> lock(status_mutex_);

    if (!loaded_) return;

    loaded_ = false;
    ready_ = false;
    to_delete_ = true; // XXX: tullaan poistamaan VAO main-threadistä

    lock.unlock();

    Debug::start()[0] << "--- " << filename_ << Debug::end();

    for (auto mesh: meshes_) {
        delete mesh;
    }
    meshes_.clear();

    // for (auto face: faces_) {
    //     delete face;
    // }
    // faces_.clear();
}

// Main-thread
void Scene::unloadVao() {
    std::unique_lock<std::mutex> lock(status_mutex_);
    // XXX: pitää estää ettei uusi load voi alkaa ennen kuin unload aivan valmis
    if (to_delete_) {
        glDeleteBuffers(2, buffers_);
        buffers_[0] = 0;
        buffers_[1] = 0;

        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;

        to_delete_ = false;
    }
}

// Main-thread
Scene::~Scene() {
    unload();
    unloadVao();
}

// Main-thread
void Scene::draw() {
    // Varmistetaan että ei yritetä poistaa kesken piirtämisen
    std::unique_lock<std::mutex> lock(status_mutex_);

    if (!ready_) return;

    glBindVertexArray(vao_);

    Shader* shader = Manager::instance().getShader("lightning");

    for (auto mesh: meshes_) {
        Material material = materials_.at(mesh->materialIndex());
        glVertexAttrib4fv(shader->attribLoc("in_color_diffuse"), glm::value_ptr(material.diffuse));
        // glVertexAttrib4fv(shader->attribLoc("in_color_ambient"), glm::value_ptr(material.ambient));
        glVertexAttrib1f(shader->attribLoc("in_color_opacity"), material.opacity);
        // glVertexAttrib3fv(shader->attribLoc("in_color_specular"), material->specular());
        mesh->draw();
    }

    glBindVertexArray(0);
}

void Scene::write() {
    if (!loaded_) return;

    if (ready_) {
        Debug::start()[2] << "Ei voida kirjoittaa sceneä talteen koska tiedot siirrettyä Vao" << Debug::end();
        return;
    }

    if (filename_.extension() != ".obj") {
        Debug::start()[2] << "Ei voida kirjoittaa sceneä talteen kuin .obj tiedoston perusteella" << Debug::end();
        return;
    }

    boost::filesystem::path outfile_path = filename_;
    outfile_path.replace_extension(".gldata");

    string temp = outfile_path.native();

    if (temp.substr(0, 10) == "assets/obj") {
        outfile_path = boost::filesystem::path(string("assets/gldata") + temp.substr(10));
    }

    boost::filesystem::create_directories(outfile_path.parent_path());

    io::filtering_ostream outfile;
    outfile.push(io::basic_zlib_compressor<>());
    outfile.push(io::basic_file_sink<char>(outfile_path.native(), std::ios_base::binary));

    GlDataHeaderUnion data;
    data.data.materials = materials_.size();
    data.data.meshes = meshes_.size();
    data.data.vertexes = gl_vertexes.size();
    data.data.faces = gl_faces.size();

    outfile.write(data.chars, GLDATAHEADER_BYTES);

    for (auto material: materials_) {
        MaterialUnion d(material);
        outfile.write(d.chars, MATERIAL_BYTES);
    }

    for (auto mesh: meshes_) {
        MeshUnion d(*mesh);
        outfile.write(d.chars, MESH_BYTES);
    }

    for (auto vertex: gl_vertexes) {
        GLVertexUnion d(vertex);
        outfile.write(d.chars, VERTEX_BYTES);
    }

    for (auto face: gl_faces) {
        GLFaceUnion d(face);
        outfile.write(d.chars, FACE_BYTES);
    }

    Debug::start()[2] << "Writing: " << outfile_path << "." << Debug::end();
}

bool Scene::collision(const glm::vec3& point, glm::vec3& movement) const {
/*
    // jossei liikuttu
    if (movement.length() == 0) return false;

    // Ei kappaleen bounding boxin sisällä
    if (!point.inside(min_, max_)) {
        return false;
    }

    double nearestDistance = 0.0;
    Face* nearest = NULL;

    glm::vec3 p(point + movement);


    // Debug::start() << "Piste " << p << Debug::end();
    for (unsigned int i = 0; i < faces_.size(); ++i) {
        Face* face = faces_.at(i);

        if (p.inside(face->min, face->max)) {

            // etäisyys kohteesta tason lähimpään pisteeseen
            double distance = p.distanceToPlane(face->normal, face->d);

            // kohde pistettä lähin piste tasolla
            Vec3 q(p - face->normal * distance);

            if (distance < CAMERA_R && (nearest == NULL || distance < nearestDistance)
             && face->isPointInside(q)) {

                nearest = face;
                nearestDistance = distance;
            }
        }

    }

    if (nearest != NULL) {
        movement += nearest->normal * (CAMERA_R - nearestDistance);
    }
*/
    return false;
}
