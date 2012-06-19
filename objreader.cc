#include <string>
using std::string;
#include <fstream>
using std::ifstream;
#include <sstream>
using std::istringstream;
using std::ostringstream;
#include <iostream>
using std::cout;
using std::endl;
#include <cmath>
#include <vector>
using std::vector;
#include <algorithm>
using std::partial_sort;
#include <utility>
using std::pair;
#include <map>
using std::map;

#include <GL3/gl3w.h>

#include <assimp/assimp.hpp>
#include <assimp/aiPostProcess.h> // Post processing flags

#include "objreader.hh"
#include "debug.hh"
#include "common.hh"
#include "material.hh"
#include "solidmaterial.hh"

#include "manager.hh"

namespace {
    int CheckGLErrors() {
        int errCount = 0;
        for(GLenum currError = glGetError(); currError != GL_NO_ERROR; currError = glGetError()) {
            Debug::start()[0] << "OpenGL error: " << currError << Debug::end();
            ++errCount;
        }
        return errCount;
    }
}

bool Scene::draw_normals_ = false;
bool Scene::draw_face_centers_ = false;

Face::Face(Vec3* a_, Vec3* b_, Vec3* c_):
    a(a_), b(b_), c(c_),
    normal((*b - *a).cross(*c - *a)),
    d(0.0),
    min(std::min(a->x, std::min(b->x, c->x)),
        std::min(a->y, std::min(b->y, c->y)),
        std::min(a->z, std::min(b->z, c->z))),
    max(std::max(a->x, std::max(b->x, c->x)),
        std::max(a->y, std::max(b->y, c->y)),
        std::max(a->z, std::max(b->z, c->z)))
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
bool Face::isPointInside(const Vec3 &point) const {
    Vec3 v0 = *c - *a;
    Vec3 v1 = *b - *a;
    Vec3 v2 = point - *a;

    double dot00 = v0.dot(v0);
    double dot01 = v0.dot(v1);
    double dot02 = v0.dot(v2);
    double dot11 = v1.dot(v1);
    double dot12 = v1.dot(v2);

    double inv_denom = 1.0 / (dot00 * dot11 - dot01 * dot01);
    double u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
    double v = (dot00 * dot12 - dot01 * dot02) * inv_denom;

    return (u >= 0) && (v >= 0) && (u + v < 1);
}


// ---------- MESH --------------------


Mesh::Mesh():
    start_face_(0),
    face_count_(0),
    material_(0),
    min_(0, 0, 0),
    max_(0, 0, 0)
{}

bool Mesh::load(const aiMesh* mesh, std::vector<GLVertex>& gl_vertexes, std::vector<GLFace>& gl_faces) {
    if (!mesh->HasNormals() || !mesh->HasPositions() || !mesh->HasFaces()) {
        Debug::start()[3] << "Meshiä \"" << mesh->mName.data << "\" ei voitu lukea koska se ei sisällä [normaaleita|sijainteja|pintoja]" << Debug::end();
        return false;
    }

    bool first = true;
    unsigned int first_vertex = gl_vertexes.size();
    for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
        const aiVector3D vertex = mesh->mVertices[j];
        const aiVector3D normal = mesh->mNormals[j];

        GLVertex gl_vertex(vertex.x, vertex.y, vertex.z);
        gl_vertex.setNormal(normal.x, normal.y, normal.z);

        if (first || vertex.x < min_.x) min_.x = vertex.x;
        if (first || vertex.x > max_.x) max_.x = vertex.x;
        if (first || vertex.y < min_.y) min_.y = vertex.y;
        if (first || vertex.y > max_.y) max_.y = vertex.y;
        if (first || vertex.z < min_.z) min_.z = vertex.z;
        if (first || vertex.z > max_.z) max_.z = vertex.z;
        first = false;

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

void Mesh::draw() const {
    CheckGLErrors();
    // Debug::start()[1] << "Meshin pinnat alkavat indeksistä " << start_face_ << Debug::end();
    glDrawElements(GL_TRIANGLES, 3 * face_count_, GL_UNSIGNED_SHORT, (char*)NULL + start_face_ * sizeof(GLFace));
}


// --------- SCENE --------------------

Scene::Scene(const string& filename):
    loaded_(false),
    filename_(filename),
    vao_(0),
    // vinx_(0),
    meshes_(),
    min_(0.0, 0.0, 0.0),
    max_(0.0, 0.0, 0.0)
{}

void Scene::load() {
    if (loaded_) {
        unload();
    }

    vector<GLVertex> gl_vertexes;
    vector<GLFace> gl_faces;
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filename_.c_str(),
        // aiProcess_CalcTangentSpace       |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    // Ei pitäisi olla rakentajassa
    if(!scene) {
        Debug::start()[0] << importer.GetErrorString() << Debug::end();
        return;
    }

    if (!scene->HasMeshes()) {
        return;
    }

    Debug::start()[1] << "Tiedostossa " << filename_ << " on " << scene->mNumMaterials << " materiaalia, " << scene->mNumMeshes << " mesh" << Debug::end();

    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        SolidMaterial* material = new SolidMaterial;

        const aiMaterial* mat = scene->mMaterials[i];

        aiColor3D color(0.0, 0.0, 0.0);
        // mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        // material->setDiffuse(color.r, color.g, color.b);

        mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
        material->setAmbient(color.r, color.g, color.b);

        // mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
        // material->setSpecular(color.r, color.g, color.b);

        // mat->Get(AI_MATKEY_COLOR_EMISSIVE, color);
        // material->setEmission(color.r, color.g, color.b);

        // float shininess = 0.0;
        // mat->Get(AI_MATKEY_SHININESS_STRENGTH, shininess);
        // material->setShinines(shininess);

        materials_.push_back(material);
    }

    bool first = true;
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        Mesh* mesh = new Mesh;
        if (mesh->load(scene->mMeshes[i], gl_vertexes, gl_faces)) {

            if (first || mesh->min().x < min_.x) min_.x = mesh->min().x;
            if (first || mesh->max().x > max_.x) max_.x = mesh->max().x;
            if (first || mesh->min().y < min_.y) min_.y = mesh->min().y;
            if (first || mesh->max().y > max_.y) max_.y = mesh->max().y;
            if (first || mesh->min().z < min_.z) min_.z = mesh->min().z;
            if (first || mesh->max().z > max_.z) max_.z = mesh->max().z;
            first = false;

            meshes_.push_back(mesh);
        } else {
            delete mesh;
        }
    }

    Debug::start()[2] << "Luettiin " << gl_vertexes.size() << " vertexiä, " << gl_faces.size() << " facea" << Debug::end();

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);


    Shader* shader = Manager::instance().getShader("lightning");

    // in_position
    glGenBuffers(1, &vertices_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_buffer_);
    glBufferData(GL_ARRAY_BUFFER, gl_vertexes.size() * sizeof(GLVertex), &gl_vertexes.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(shader->attribLoc("in_Position"), 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), 0);

    // in normal
    // GLuint normal_loc = glGetAttribLocation(Manager::instance().getShader("lightning")->handle(), "in_Normal");
    // glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (char*)0 + 3 * sizeof(float));
    // glEnableVertexAttribArray(1);

    GLuint vinx;
    glGenBuffers(1, &vinx);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gl_faces.size() * sizeof(GLFace), &gl_faces.front(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    // Rakennetaan luetuista vertexeistä ja pinnoista omia vertexeitä ja pintoja
    // joilla vähän enempi ominaisuuksia. (törmäystarkistusta varten)
    // vanhat ovat turhia tämän jälkeen (luettu jo vbo muistiin joten ei tarvita alkup
    // tietoja piirtämiseen)
    vector<Vec3*> vertexes;
    for (unsigned int i = 0; i < gl_vertexes.size(); ++i) {
        GLVertex v = gl_vertexes.at(i);
        vertexes.push_back(new Vec3(v.x, v.y, v.z));
    }

    for (unsigned int i = 0; i < gl_faces.size(); ++i) {
        Vec3* a = vertexes.at(gl_faces.at(i).a);
        Vec3* b = vertexes.at(gl_faces.at(i).b);
        Vec3* c = vertexes.at(gl_faces.at(i).c);
        faces_.push_back(new Face(a, b, c));
    }

    loaded_ = true;
}

void Scene::unload() {
    loaded_ = false;
    // filename_ = "";
    min_ = Vec3(0, 0, 0);
    max_ = Vec3(0, 0, 0);

    for (unsigned int i = 0; i < meshes_.size(); ++i) {
        delete meshes_.at(i);
    }
    meshes_.clear();

    for (unsigned int i = 0; i < faces_.size(); ++i) {
        delete faces_.at(i);
    }
    faces_.clear();

    glDeleteVertexArrays(1, &vao_);
}


Scene::~Scene() {
    unload();
}


void Scene::draw() const {
    glBindVertexArray(vao_);

    Shader* shader = Manager::instance().getShader("lightning");

    for (unsigned int i = 0; i < meshes_.size(); ++i) {
        Mesh* mesh = meshes_.at(i);
        SolidMaterial* material = dynamic_cast<SolidMaterial*>(materials_.at(mesh->materialIndex()));
        glVertexAttrib3fv(shader->attribLoc("in_Color"), material->diffuse());
        mesh->draw();
    }

    glBindVertexArray(0);
}


bool Scene::collision(const Vec3& point, Vec3& movement) const {
    // jossei liikuttu
    if (movement.length() == 0) return false;

    // Ei kappaleen bounding boxin sisällä
    if (!point.inside(min_, max_)) {
        return false;
    }

    double nearestDistance = 0.0;
    Face* nearest = NULL;

    Vec3 p(point + movement);


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

    return false;
}

/*
bool Scene::rayCollision(const Vec3& point, const Vec3& ray, float& distance) {
    double nearestDistance = 0.0;
    Face* nearest = NULL;

    // Debug::start() << "Ray " << ray << Debug::end();

    Vec3 p(point);
    Vec3 q(point + ray);
    Vec3 pq(ray);

    for (unsigned int i = 0; i < faces_.size(); ++i) {
        Face* face = faces_.at(i);

        double npq = face->normal.dot(ray);
        if (npq != 0) {
            double t = - (face->normal.dot(p) + face->d) / npq;
            // säde leikkaa tason
            if (t >= 0) {
                Vec3 intersect(point + ray * t);
                // Debug::start() << "Leikkaus piste " << intersect << Debug::end();
                if (face->isPointInside(intersect)) {
                    // Debug::start() << "Piste on pinnan sisällä" << Debug::end();
                    if (nearest == NULL || t < nearestDistance) {
                        nearestDistance = t;
                        nearest = face;
                    }
                }
            }
        }
    }

    distance = nearestDistance;
    return nearest != NULL;
}
*/
