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

bool ObjReader::drawNormals_ = false;
bool ObjReader::drawFaceCenters_ = false;

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

Toteutin 3 ulotteisen kdtree:n vertexeille mutta
se ei jonkun takia palauttanut lähintä pistettä kovin
luotettavasti. Kaikkien pintojenkin läpikäynti tuntuu olevan kohtuu nopeata.
*/
ObjReader::ObjReader(const char* filename):
    vbo_(0),
    vinx_(0),
    faceCount_(0),
    faces_(),
    // vertexes_(),
    // tree_(NULL),
    // best_(NULL),
    min_(0.0, 0.0, 0.0),
    max_(0.0, 0.0, 0.0),
    p(0.0, 0.0, 0.0),
    q(0.0, 0.0, 0.0),
    tormatty_(NULL)
    {

    vector<GLVertex> glVertexes;
    vector<Vec3> normals;
    vector<GLFace> glFaces;

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
    // glGenVertexArrays(1, &vbo_); // XXX oiskoha paree näitä käyttäen
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
    vector<Vec3*> vertexes;
    for (unsigned int i = 0; i < glVertexes.size(); ++i) {
        GLVertex v = glVertexes.at(i);
        vertexes.push_back(new Vec3(v.x, v.y, v.z));
    }

    for (unsigned int i = 0; i < glFaces.size(); ++i) {
        Vec3* a = vertexes.at(glFaces.at(i).a);
        Vec3* b = vertexes.at(glFaces.at(i).b);
        Vec3* c = vertexes.at(glFaces.at(i).c);
        faces_.push_back(new Face(a, b, c));
        // vertexes.at(glFaces.at(i).a)->appendFace(a, b, c);
        // vertexes.at(glFaces.at(i).b)->appendFace(a, b, c);
        // vertexes.at(glFaces.at(i).c)->appendFace(a, b, c);
    }

    // tree_ = kdtree(0, vertexes_.size() - 1, 0);
}

// Vertex* ObjReader::kdtree(unsigned int start,
//                           unsigned int end,
//                           unsigned int depth) {
//     if (end < start) {
//         return NULL;
//     }

//     if (start == end) return vertexes_.at(start);

//     // Ollaan kiinnostuttu mediaanista jonkin akselin suhteen
//     unsigned int median = start + (end - start) / 2;

//     // Aika nopea tää oli kyll vaikka järjestäis sort:lla koko homman
//     partial_sort(vertexes_.begin() + start,
//                  vertexes_.begin() + median + 1, // hmm tarviikohan tuota +1?
//                  vertexes_.begin() + end,
//                  VertexCompare(static_cast<VertexCompare::SortType>(depth % 3)));

//     Vertex* uusi = vertexes_.at(median);

//     uusi->lesser = kdtree(start, median - 1, depth + 1);
//     uusi->greater = kdtree(median + 1, end, depth + 1);

//     return uusi;
// }

// void ObjReader::travel(Vertex* i, const Vec3& point, Vertex*& best, float& bestWeight, unsigned int depth) {
//     if (i == NULL) return;

//     Vertex* next = NULL;
//     Vertex* other = NULL;
//     if ((depth % 3 == 0 && point.x < i->x)
//      || (depth % 3 == 1 && point.y < i->y)
//      || (depth % 3 == 2 && point.z < i->z)) {
//         next = i->lesser;
//         other = i->greater;
//     } else {
//         next = i->greater;
//         other = i->lesser;
//     }
//     if (next != NULL) travel(next, point, best, bestWeight, depth + 1);

//     // vika
//     float weight = i->weightBetween(point);
//     if (best == NULL || weight < bestWeight) {
//         best = i;
//         bestWeight = weight;
//     }

//     // pitäisikö tarkistaa myös toinen haara?
//     if (other != NULL) {
//         weight = other->weightBetween(point);
//         if (weight < bestWeight) {
//             travel(other, point, best, bestWeight, depth + 1);
//         }
//     }
// }

// Vertex* ObjReader::nearestPoint(const Vec3& point)
// {
//     float bestWeight = 0;
//     best_ = NULL;
//     travel(tree_, point, best_, bestWeight, 0);
//     return best_;
// }

void ObjReader::draw() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx_);
    // glInterleavedArrays GL_N3F_V3F / GL_T2F_N3F_V3F

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof (GLVertex), NULL);

    glEnableClientState(GL_NORMAL_ARRAY);
    // Normaalit structissa 3:n floatin jälkeen
    glNormalPointer(GL_FLOAT, sizeof (GLVertex), (char*)NULL + 3 * sizeof(float));

    // glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    // Tekstuurikoordinaatit structissa 6:n floattia alun jälkeen
    // glTexCoordPointer(2, GL_FLOAT, sizeof (Vertex), &vertexes_.front() + 6 * sizeof(float));

    glDrawElements(GL_TRIANGLES, 3 * faceCount_, GL_UNSIGNED_INT, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    // if (best_ != NULL) {
    //     GLfloat diffuse[] = {1.0, 0.0, 0.0, 1.0};
    //     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    //     glPushMatrix();
    //     glTranslatef(best_->x,
    //                  best_->y,
    //                  best_->z);
    //     glutSolidSphere(0.05, 8, 8);
    //     glPopMatrix();
    // }


    if (drawFaceCenters_ || drawNormals_) {
        if (drawNormals_) glBegin(GL_LINES);
        else              glBegin(GL_POINTS);

        GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, diffuse);
        // glBegin(GL_POINTS);
        for (unsigned int i = 0; i < faces_.size(); ++i) {
            Face* f = faces_.at(i);
            Vec3 a(f->a->x / 3 + f->b->x / 3 + f->c->x / 3,
                   f->a->y / 3 + f->b->y / 3 + f->c->y / 3,
                   f->a->z / 3 + f->b->z / 3 + f->c->z / 3);
            glVertex3f(a.x, a.y, a.z);

            if (drawNormals_) {
                Vec3 b(a + f->normal);
                glVertex3f(b.x, b.y, b.z);
            }
        }
        glEnd();

        glBegin(GL_LINES);
            glVertex3f(p.x, p.y, p.z);
            glVertex3f(q.x, q.y, q.z);
        glEnd();

        if (tormatty_ != NULL) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            glBegin(GL_TRIANGLES);
                glVertex3f(tormatty_->a->x, tormatty_->a->y, tormatty_->a->z);
                glVertex3f(tormatty_->b->x, tormatty_->b->y, tormatty_->b->z);
                glVertex3f(tormatty_->c->x, tormatty_->c->y, tormatty_->c->z);
            glEnd();
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        }

        GLfloat fuu[] = {0.0, 0.0, 0.0, 0.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, fuu);
    }

}

bool ObjReader::collision(const Vec3& point, Vec3& movement, unsigned int depth) {
    if (depth > 10) return true;
    // jossei liikuttu
    if (movement.length() == 0) return false;

    // Ei kappaleen bounding boxin sisällä
    // if (!point.inside(min_, max_)) {
    //     return false;
    // }

    p = point;
    q = point + movement;
    // Debug::start() << "Liikutaan " << p << " + " << movement << " -> " << q << Debug::end();

    // Vertex* v = nearestPoint(point);

    // for (unsigned int i = 0; i < vertexes_.size(); ++i) {

    //     Vertex* v = vertexes_.at(i);
    //     Vertex::Face* face = v->faces;
    //     while (face != NULL) {
    for (unsigned int i = 0; i < faces_.size(); ++i) {
        Face* face = faces_.at(i);
        // tähän tarkistus että törmääkö pintaan
        // kolmion boundingbox
        float dp = p.distanceToPlane(face->normal, face->d);
        float dq = q.distanceToPlane(face->normal, face->d);

        Debug::start() << "p " << p << " etäisyys tasoon " << dp << Debug::end();
        Debug::start() << "q " << q << " etäisyys tasoon " << dq << Debug::end();

        // float t = - (face->normal * p )

        // float d = (point.dot(face->normal) - Vec3(face->a->x, face->a->y, face->a->z).dot(face->normal)) / face->normal.length();
        // if (d < 1) {
        // jos normaali osuu jossain pisteessä tasoon
        // float npq = face->normal.dot(movement);
        // if (npq != 0) {
            // XXX: tarkemmat tarkastukset
            // float t = - (face->normal.dot(point) + face->d) / npq;
            // if (t >= 0.0 && t <= 1.0) {
                Debug::start() << "Törmätään tasoon (" << *(face->a) << ") / (" << *(face->b) << ") / (" << *(face->c) << ")" << Debug::end();
                tormatty_ = face;
                // Vec3 
                movement += face->normal * 0.1;
                // liikutaan törmäys pisteeseen + pinnan normaalin suuntaan "kameran etäisyydelle"
                return collision(point, movement, depth + 1);
                return true;
            // }
        // }
        // }
        face = face->next;
    }

    // }

    // Debug::start() << "Ei törmätty mihinkään" << Debug::end();
    return false;
}
