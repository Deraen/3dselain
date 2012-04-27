#include <string>
using std::string;
#include <fstream>
using std::ifstream;
#include <sstream>
using std::istringstream;
#include <iostream>
using std::cout;
using std::endl;
#include <cmath>
// using std::min;
// using std::max;
#include <vector>
using std::vector;
#include <algorithm>
using std::partial_sort;

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>

#include "objreader.hh"
#include "debug.hh"

namespace {
    unsigned int string2uint(const string& c) {
        istringstream a(c);
        unsigned int b = 0;
        a >> b;
        return b;
    }

}

/*
Olettaa että tiedostossa on vain yksi objekti.
*/
ObjReader::ObjReader(const char* filename):
    vbo_(0),
    vinx_(0),
    faceCount_(0),
    vertexes_(),
    tree_(NULL),
    best_(NULL),
    min_(0.0, 0.0, 0.0),
    max_(0.0, 0.0, 0.0)
    {

    // vector<GLVertex> glVertexes;
    vector<Vec3> normals;
    // vector<GLFace> glFaces;

    ifstream file(filename);

    // XXX: ois ehk hyvä tarkistaa että eri asioita luetaan yhtä monta
    bool stop = false;
    bool first = true;
    while(!stop && file) {
        string line;
        getline(file, line);
        if (line.length() == 0
         || line.at(0) == '#') continue; // tyhjä rivi tai kommentti mennään seuraavaan

        istringstream lines(line);
        string eka;
        lines >> eka;

        if (eka == "v") { // vertexit
            float x, y, z;
            lines >> x >> y >> z;
            glVertexes.push_back(GLVertex(x, y, z));

            if (first || x < min_.x) min_.x = x;
            if (first || x > max_.x) max_.x = x;
            if (first || y < min_.y) min_.y = y;
            if (first || y > max_.y) max_.y = y;
            if (first || z < min_.z) min_.z = z;
            if (first || z > max_.z) max_.z = z;
        } else if (eka == "vt") { // tekstuuri
        } else if (eka == "vn") { // normaali
            // luetaan väliaikaisesti talteen tuonne (koska ennen facejen lukemista ei tiedetä mihin vertexiin liittyy...)
            float x, y, z;
            lines >> x >> y >> z;
            normals.push_back(Vec3(x, y, z).normalize());
        } else if (eka == "f") { // facet
            // facet tiedostossa:
            // f vertex/tekstuurit/normaali vertex/tekstuuri/normaali vertex/tekstuuri/normaali
            // vertexeille tallennetaan tässä noista aikaisemmin luetuista normaaleista joku numeron perusteella
            // koska opengl tahtoo että normaalit liittyvät vertexeihin

            unsigned int v[3] = {0, 0, 0};
            for (unsigned int i = 0; i < 3; ++i) {
                string face;
                lines >> face;
                istringstream faces(face);

                if (normals.size() > 0) {
                    string a;
                    getline(faces, a, '/');
                    unsigned int vertex = string2uint(a) - 1;
                    getline(faces, a, '/');
                    getline(faces, a, '/');
                    unsigned int normal = string2uint(a) - 1;

                    glVertexes.at(vertex).nx = normals.at(normal).x;
                    glVertexes.at(vertex).ny = normals.at(normal).y;
                    glVertexes.at(vertex).nz = normals.at(normal).z;
                    v[i] = vertex;
                } else {
                    Debug::start()[3] << "Normaali puuttuu" << Debug::end();
                    stop = true;
                }
            }

            // Debug::start()[3] << "Pinta " << v[0] << ", " << v[1] << ", " << v[2] << Debug::end();
            glFaces.push_back(GLFace(v[0], v[1], v[2]));
        } else if (eka == "s") { // shading
        } else if (eka == "o") { // objekti
        } else if (eka == "mtllib") {
        } else if (eka == "usemtl") { // pitäis käyttää jotai materiaalia
        } else {
            Debug::start()[3] << "Tunnistamaton rivi .obj tiedostossa " << eka << Debug::end();
            stop = true;
        }

        first = false;
    }

    Debug::start()[0] << "Luettiin " << glVertexes.size() << " vertexiä, sizeof GLVertex: " << sizeof(GLVertex) << Debug::end();
    Debug::start()[0] << "Luettiin " << glFaces.size() << " facea, sizeof GLFace: " << sizeof(GLFace) << Debug::end();

    // Talletetaan glVertexit ja glFacet VBO:hon.
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, glVertexes.size() * sizeof(GLVertex), &glVertexes.front(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // for (unsigned int i = 0; i < glFaces.size(); ++i) {
    //     Debug::start()[1] << "Pinta " << i << " " << &glFaces.at(i) << Debug::end();
    // }

    glGenBuffers(1, &vinx_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, glFaces.size() * sizeof(GLFace), &glFaces.front(), GL_STATIC_DRAW);

    GLFace* temp = NULL;
    glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, faceCount_ * sizeof(GLFace), temp);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // ainut mität tarvitaan jatkossa piirtoon
    faceCount_ = glFaces.size();

    // for (unsigned int i = 0; i < faceCount_; ++i) {
    //     unsigned int a = temp[i].a;
    //     unsigned int b = temp[i].b;
    //     unsigned int c = temp[i].c;
    //     Debug::start() << "Pinta " << i << " " << a << " - " << b << " - " << c << Debug::end();
    // }

    // Rakennetaan luetuista vertexeistä ja pinnoista omia vertexeitä
    // joilla vähän enempi ominaisuuksia.
    // vanhat ovat turhia tämän jälkeen (luettu jo vbo muistiin joten ei tarvita alkup
    // tietoja piirtämiseen)
    for (unsigned int i = 0; i < glVertexes.size(); ++i) {
        GLVertex v = glVertexes.at(i);
        vertexes_.push_back(new Vertex(v.x, v.y, v.z));
    }

    for (unsigned int i = 0; i < glFaces.size(); ++i) {
        Vertex* a = vertexes_.at(glFaces.at(i).a);
        Vertex* b = vertexes_.at(glFaces.at(i).b);
        Vertex* c = vertexes_.at(glFaces.at(i).c);
        vertexes_.at(glFaces.at(i).a)->appendFace(a, b, c);
        vertexes_.at(glFaces.at(i).b)->appendFace(a, b, c);
        vertexes_.at(glFaces.at(i).c)->appendFace(a, b, c);
    }

    tree_ = kdtree(0, vertexes_.size() - 1, 0);
}

Vertex* ObjReader::kdtree(unsigned int start,
                          unsigned int end,
                          unsigned int depth) {
    if (end < start) {
        return NULL;
    }

    if (start == end) return vertexes_.at(start);

    // Ollaan kiinnostuttu mediaanista jonkin akselin suhteen
    unsigned int median = start + (end - start) / 2;

    // Aika nopea tää oli kyll vaikka järjestäis sort:lla koko homman
    partial_sort(vertexes_.begin() + start,
                 vertexes_.begin() + median + 1, // hmm tarviikohan tuota +1?
                 vertexes_.begin() + end,
                 VertexCompare(static_cast<VertexCompare::SortType>(depth % 3)));

    Vertex* uusi = vertexes_.at(median);

    uusi->lesser = kdtree(start, median - 1, depth + 1);
    uusi->greater = kdtree(median + 1, end, depth + 1);

    return uusi;
}

void ObjReader::travel(Vertex* i, const Vec3& point, Vertex*& best, float& bestWeight, unsigned int depth) {
    if (i == NULL) return;

    Vertex* next = NULL;
    Vertex* other = NULL;
    if ((depth % 3 == 0 && point.x < i->x)
     || (depth % 3 == 1 && point.y < i->y)
     || (depth % 3 == 2 && point.z < i->z)) {
        next = i->lesser;
        other = i->greater;
    } else {
        next = i->greater;
        other = i->lesser;
    }
    if (next != NULL) travel(next, point, best, bestWeight, depth + 1);

    // vika
    float weight = i->weightBetween(point);
    if (best == NULL || weight < bestWeight) {
        best = i;
        bestWeight = weight;
    }

    // pitäisikö tarkistaa myös toinen haara?
    if (other != NULL) {
        weight = other->weightBetween(point);
        if (weight < bestWeight) {
            travel(other, point, best, bestWeight, depth + 1);
        }
    }
}

Vertex* ObjReader::nearestPoint(const Vec3& point)
{
    float bestWeight = 0;
    best_ = NULL;
    travel(tree_, point, best_, bestWeight, 0);
    return best_;
}

void ObjReader::draw() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx_);
    // glInterleavedArrays GL_N3F_V3F / GL_T2F_N3F_V3F

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof (Vertex), NULL);

    glEnableClientState(GL_NORMAL_ARRAY);
    // Normaalit structissa 3:n floatin jälkeen
    glNormalPointer(GL_FLOAT, sizeof (Vertex), (char*)NULL + 3 * sizeof(float));

    // glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    // Tekstuurikoordinaatit structissa 6:n floattia alun jälkeen
    // glTexCoordPointer(2, GL_FLOAT, sizeof (Vertex), &vertexes_.front() + 6 * sizeof(float));

    glDrawElements(GL_TRIANGLES, 3 * faceCount_, GL_UNSIGNED_INT, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    if (best_ != NULL) {
        GLfloat diffuse[] = {1.0, 0.0, 0.0, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glPushMatrix();
        glTranslatef(best_->x,
                     best_->y,
                     best_->z);
        glutSolidSphere(0.05, 8, 8);
        glPopMatrix();
    }
}

bool ObjReader::collision(const Vec3& point, Vec3& movement) {
    // Ei kappaleen bounding boxin sisällä
    if (!point.inside(min_, max_)) {
        return false;
    }

    Vertex* v = nearestPoint(point);

    Vertex::Face* face = v->faces;
    while (face != NULL) {
        // tähän tarkistus että törmääkö pintaan
        movement += face->normal * 0.1;
        return true;
    }

    return false;
}
