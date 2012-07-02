#include <string>
using std::string;
#include <vector>
using std::vector;

#include <GL3/gl3w.h>

#include <assimp/assimp.hpp>
#include <assimp/aiPostProcess.h> // Post processing flags

#include "scene.hh"
#include "debug.hh"
#include "common.hh"
#include "material.hh"
#include "solidmaterial.hh"

#include "manager.hh"

// ---------- MESH --------------------

Scene::Mesh::Mesh():
    start_face_(0),
    face_count_(0),
    material_(0)//,
    // min_(0, 0, 0),
    // max_(0, 0, 0)
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
    to_delete_(true),
    filename_(filename),
    vao_(0),
    // vinx_(0),
    meshes_()
{
    // XXX: center, min, max lataus tässä jostai omasta tiedostosta
}

// Loader-thread
void Scene::load() {
    std::unique_lock<std::mutex> lock(status_mutex_);
    if (to_delete_) return;
    if (loaded_) return;

    to_delete_ = false;
    ready_ = false;
    lock.unlock();

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filename_.c_str(),
        // aiProcess_CalcTangentSpace       |
        aiProcess_Triangulate |
        // aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    if (!scene) {
        Debug::start()[0] << importer.GetErrorString() << Debug::end();
        return;
    }

    if (!scene->HasMeshes()) {
        return;
    }

    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        SolidMaterial* material = new SolidMaterial;

        const aiMaterial* mat = scene->mMaterials[i];

        aiColor3D color(0.0, 0.0, 0.0);
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        material->setDiffuse(color.r, color.g, color.b);

        mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
        material->setAmbient(color.r, color.g, color.b);

        mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
        material->setSpecular(color.r, color.g, color.b);

        float value;
        mat->Get(AI_MATKEY_OPACITY, value);
        material->setOpacity(value);

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
                      << scene->mNumMaterials << " Materials. "
                      << scene->mNumMeshes << " Meshes. "
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

    for (auto material: materials_) {
        delete material;
    }
    materials_.clear();

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
        SolidMaterial* material = dynamic_cast<SolidMaterial*>(materials_.at(mesh->materialIndex()));
        glVertexAttrib4fv(shader->attribLoc("in_color_diffuse"), material->diffuse());
        glVertexAttrib4fv(shader->attribLoc("in_color_ambient"), material->ambient());
        glVertexAttrib1f(shader->attribLoc("in_color_opacity"), material->opacity());
        // glVertexAttrib3fv(shader->attribLoc("in_color_specular"), material->specular());
        mesh->draw();
    }

    glBindVertexArray(0);
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
