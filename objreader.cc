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

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>

#include "objreader.hh"
#include "debug.hh"
#include "common.hh"

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

Törmäysten etsintä tuntuu toimivan tarpeaksi nopeasti ilman kdtree.
*/
ObjReader::ObjReader(const string& dir, const string& filename):
    vbo_(0),
    vinx_(0),
    faces_(),
    min_(0.0, 0.0, 0.0),
    max_(0.0, 0.0, 0.0),
    colors_()
    {

    vector<GLVertex> glVertexes;
    vector<Vec3> normals;
    vector<pair<float, float> > textureCoords;
    vector<GLFace> glFaces;
    Vec4 used_color(0.0, 0.0, 0.0, 1.0);

    // map<string, Vec4> colors;
    string path = dir;
    path.append("/");
    path.append(filename);

    ifstream file(path.c_str());

    bool stop = false;
    bool first = true;
    while(!stop && file) {
        string line;
        getline(file, line);
        if (line.length() == 0 || line.at(0) == '#' || line.at(0) == '\n') continue; // tyhjä rivi tai kommentti mennään seuraavaan

        istringstream lines(line);
        string eka;
        lines >> eka;

        if (eka == "v") { // vertexit
            double x, y, z;
            lines >> x >> y >> z;
            glVertexes.push_back(GLVertex(x, y, z));

            if (first || x < min_.x) min_.x = x;
            if (first || x > max_.x) max_.x = x;
            if (first || y < min_.y) min_.y = y;
            if (first || y > max_.y) max_.y = y;
            if (first || z < min_.z) min_.z = z;
            if (first || z > max_.z) max_.z = z;
        } else if (eka == "vt") { // tekstuuri
            float u, v;
            lines >> u >> v;
            textureCoords.push_back(pair<float,float>(u, v));
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
                    unsigned int texture = string2uint(a) - 1;
                    getline(faces, a, '/');
                    unsigned int normal = string2uint(a) - 1;

                    glVertexes.at(vertex).nx = normals.at(normal).x;
                    glVertexes.at(vertex).ny = normals.at(normal).y;
                    glVertexes.at(vertex).nz = normals.at(normal).z;
                    if (texture != static_cast<unsigned int>(-1)) {
                        glVertexes.at(vertex).u = textureCoords.at(texture).first;
                        glVertexes.at(vertex).v = textureCoords.at(texture).second;
                    }
                    v[i] = vertex;

                    glVertexes.at(vertex).setColor(used_color);
                } else {
                    Debug::start()[3] << "Normaali puuttuu" << Debug::end();
                    stop = true;
                }
            }

            glFaces.push_back(GLFace(v[0], v[1], v[2]));
        } else if (eka == "s") { // shading
        } else if (eka == "o") { // objekti
        } else if (eka == "g") {
        } else if (eka == "mtllib") {
            string material_file;
            lines >> material_file;

            readMaterials(dir, material_file);
        } else if (eka == "usemtl") { // pitäis käyttää jotai materiaalia
            string color_name;
            lines >> color_name;
            map<string, Vec4*>::iterator it = colors_.find(color_name);
            if (it != colors_.end()) {
                used_color = *(it->second);
            } else {
                map<string, Vec4*>::iterator it = colors_.find("default");
                if (it != colors_.end()) {
                    used_color = *(it->second);
                } else {
                    Debug::start()[3] << "Väriä " << color_name << " tai default ei löytynyt?" << Debug::end();
                    stop = true;
                }
            }
        } else {
            Debug::start()[3] << "Tunnistamaton rivi .obj tiedostossa \"" << eka << "\"" << Debug::end();
            Debug::start()[2] << line << Debug::end();
            stop = true;
        }

        first = false;
    }

    min_ -= Vec3(CAMERA_R, CAMERA_R, CAMERA_R);
    max_ += Vec3(CAMERA_R, CAMERA_R, CAMERA_R);

    Debug::start()[1] << "Malli: " << filename << Debug::end();
    Debug::start()[2] << "Luettiin " << glVertexes.size() << " vertexiä" << Debug::end();
    Debug::start()[2] << "Luettiin " << glFaces.size() << " facea" << Debug::end();

    // Talletetaan glVertexit ja glFacet VBO:hon.
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, glVertexes.size() * sizeof(GLVertex), &glVertexes.front(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vinx_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, glFaces.size() * sizeof(GLFace), &glFaces.front(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
    }

}

ObjReader::~ObjReader() {
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &vinx_);
}


void ObjReader::readMaterials(const std::string& dir, const string& filename) {

    string path = dir;
    path.append("/");
    path.append(filename);

    Debug::start()[2] << "Luetaan värit tiedostosta " << filename << Debug::end();

    ifstream file(path.c_str());

    bool stop = false;
    // bool first = true;
    string name;
    while(!stop && file) {
        string line;
        getline(file, line);
        if (line.length() == 0
         || line.at(0) == '#') continue; // tyhjä rivi tai kommentti mennään seuraavaan

        istringstream lines(line);
        string eka;
        lines >> eka;

        if (eka == "newmtl") {
            if (!(lines >> name)) {
                name = "default";
            }
            // Debug::start()[1] << "Löydetty väri " << name << Debug::end();
        } else if (eka == "Kd") { // Diffuusi väri.
            float r, g, b, a;
            a = 1.0;
            lines >> r >> g >> b;

            colors_[name] = new Vec4(r, g, b, a);
        } else if (eka == "Ka") {
            // float r, g, b, a;
            // a = 1.0;
            // lines >> r >> g >> b;

            // colors_[name] = new Vec4(r, g, b, a);
        } else if (eka == "map_Kd" || eka == "Ns" || eka == "Ks" || eka == "Ni" || eka == "d" || eka == "illum" || eka == "Tr" || eka == "Tf" || eka == "Kf" || eka == "Ke" || eka == "map_bump") {
        } else {
            Debug::start()[3] << "Tunnistamaton rivi .mtl tiedostossa " << eka << Debug::end();
            stop = true;
        }

        // first = false;
    }

}


void ObjReader::draw() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx_);
    // glInterleavedArrays GL_N3F_V3F / GL_T2F_N3F_V3F

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_DOUBLE, sizeof (GLVertex), NULL);

    glEnableClientState(GL_NORMAL_ARRAY);
    // Normaalit structissa 3:n floatin jälkeen
    glNormalPointer(GL_FLOAT, sizeof (GLVertex), (char*)NULL + 3 * sizeof(double));

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    // Tekstuurikoordinaatit structissa 6:n floattia alun jälkeen
    glTexCoordPointer(2, GL_FLOAT, sizeof (GLVertex), (char*)NULL + 3 * sizeof(double) + 3 * sizeof(float));

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_FLOAT, sizeof (GLVertex), (char*)NULL + 3 * sizeof(double) + 5 * sizeof(float));

    glDrawElements(GL_TRIANGLES, 3 * faces_.size(), GL_UNSIGNED_INT, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    if (drawFaceCenters_ || drawNormals_) {
        if (drawNormals_) glBegin(GL_LINES);
        else              glBegin(GL_POINTS);

        GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, diffuse);
        // glBegin(GL_POINTS);
        for (unsigned int i = 0; i < faces_.size(); ++i) {
            Face* f = faces_.at(i);
            Vec3 a(*(f->a) / 3.0 + *(f->b) / 3.0 + *(f->c) / 3.0);
            glVertex3d(a.x, a.y, a.z);

            if (drawNormals_) {
                Vec3 b(a + f->normal);
                glVertex3d(b.x, b.y, b.z);
            }
        }
        glEnd();

        GLfloat fuu[] = {0.0, 0.0, 0.0, 0.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, fuu);
    }

}

bool ObjReader::collision(const Vec3& point, Vec3& movement) {
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


bool ObjReader::rayCollision(const Vec3& point, const Vec3& ray, float& distance) {
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

