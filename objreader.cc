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

    template <class T>
    inline void glBufferData(GLenum target, const std::vector<T>& v, GLenum usage) {
        glBufferData(target, v.size() * sizeof(T), &v.front(), usage);
    }

}

FaceEstimate::FaceEstimate(Vertex* a_, Vertex* b_, Vertex* c_):
    a(a_), b(b_), c(c_),
    min(std::min(a->x, std::min(b->x, c->x)),
        std::min(a->x, std::min(b->x, c->x)),
        std::min(a->x, std::min(b->x, c->x))),
    max(std::min(a->x, std::min(b->x, c->x)),
        std::min(a->x, std::min(b->x, c->x)),
        std::min(a->x, std::min(b->x, c->x))),
    // XXX: ruma arvio.
    // Kolmion keskipiste olisi parempi. Ehkä turha koska kuitenkin
    // sopivilla kolmioilla väärä kolmio voi olla lähempänä.
    medium(min.x + (max.x - min.x) / 2,
           min.y + (max.y - min.y) / 2,
           min.z + (max.z - min.z) / 2),
    normal(0.0, 0.0, 0.0)
{
    Debug::start()[0] << "Kolmio (" << a->x << "," << a->y << "," << a->z << "), ("
                   << b->x << "," << b->y << "," << b->z << "), ("
                   << c->x << "," << c->y << "," << c->z << ")" << Debug::end();
    // // keskijana vertexistä a, janalle b ja c välissä
    // Vec3 bc((b->x - c->x) / 2,
    //         (b->y - c->y) / 2,
    //         (b->z - c->z) / 2);
    // // keskijana vertexistä b, janalle a ja c välissä
    // Vec3 ac((a->x - c->x) / 2,
    //         (a->y - c->y) / 2,
    //         (a->z - c->z) / 2);

    Debug::start()[0] << "keskipiste " << medium.x << "," << medium.y << "," << medium.z << Debug::end();

    Vec3 v(b->x - a->x,
           b->y - a->y,
           b->z - a->z);

    Vec3 u(c->x - a->x,
           c->y - a->y,
           c->z - a->z);

    normal = v * u;
    normal.normalize();
    Debug::start()[0] << "Normaali " << normal.x << "," << normal.y << "," << normal.z << Debug::end();

    // Normaali
}

ObjReader::ObjReader(const char* filename):
    vbo_(0), vinx_(0) {
    std::vector<Vec3> normals;

    ifstream file(filename);

    // XXX: ois ehk hyvä tarkistaa että eri asioita luetaan yhtä monta
    while(file) {
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
            vertexes_.push_back(Vertex(x, y, z));
        } else if (eka == "vt") { // tekstuuri
        } else if (eka == "vn") { // normaali
            // luetaan väliaikaisesti talteen tuonne (koska ennen facejen lukemista ei tiedetä mihin vertexiin liittyy...)
            float x, y, z;
            lines >> x >> y >> z;
            Vec3 a(x, y, z);
            a.normalize();
            normals.push_back(a);
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
                    getline(faces, a, '/'); // textuuri koord
                    getline(faces, a, '/');
                    unsigned int normal = string2uint(a) - 1;

                    v[i] = vertex;
                    vertexes_.at(vertex).nx = normals.at(normal).x;
                    vertexes_.at(vertex).ny = normals.at(normal).y;
                    vertexes_.at(vertex).nz = normals.at(normal).z;
                } else {
                    cout << "NORMAALIT PUUTTUU" << endl;
                }
            }

            faces_.push_back(Face(v[0], v[1], v[2]));
        } else if (eka == "s") { // shading käyttöön...
        } else if (eka == "o") { // objekti
        } else if (eka == "mtllib") {
        } else if (eka == "usemtl") { // pitäis käyttää jotai materiaalia
        } else {
            cout << "Tunnistamaton rivi .obj tiedostossa " << eka << endl;
        }

    }

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertexes_, GL_STATIC_DRAW); // overloaded vectorille: objreader.hh
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vinx_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces_, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    tree_ = new FaceEstimate(&vertexes_.at(faces_.at(0).a),
                             &vertexes_.at(faces_.at(0).b),
                             &vertexes_.at(faces_.at(0).c));
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

    glDrawElements(GL_TRIANGLES, 3 * faces_.size(), GL_UNSIGNED_INT, NULL);//&faces_.front());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

