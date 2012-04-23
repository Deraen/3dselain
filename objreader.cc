#include <string>
using std::string;
#include <fstream>
using std::ifstream;
#include <sstream>
using std::istringstream;
#include <iostream>
using std::cout;
using std::endl;

#include "objreader.hh"

namespace {
	unsigned int string2uint(const string& c) {
		istringstream a(c);
		unsigned int b = 0;
		a >> b;
		return b;
	}
}

ObjReader::ObjReader(const char* filename) {
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
			normals_.push_back(Vec3(x, y, z));
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

				if (normals_.size() > 0) {
					string a;
					getline(faces, a, '/');
					unsigned int vertex = string2uint(a) - 1;
					getline(faces, a, '/'); // textuuri koord
					getline(faces, a, '/');
					unsigned int normal = string2uint(a) - 1;

					v[i] = vertex;
					vertexes_.at(vertex).nx = normals_.at(normal).x;
					vertexes_.at(vertex).ny = normals_.at(normal).y;
					vertexes_.at(vertex).nz = normals_.at(normal).z;
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
}

    // glGenBuffers(1, &vbo_);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    // // glBufferData(GL_ARRAY_BUFFER, sizeof (struct Vertex) * vertexCount_, vertexes_, GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, vertexes_, GL_STATIC_DRAW);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // glGenBuffers(1, &vinx_);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx_);
    // // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (struct Face) * faceCount_, faces_, GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, faces_, GL_STATIC_DRAW);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

void ObjReader::draw() {
    // glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx_);
	// glInterleavedArrays GL_N3F_V3F / GL_T2F_N3F_V3F

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof (Vertex), &vertexes_.front());

	glEnableClientState(GL_NORMAL_ARRAY);
	// Normaalit structissa 3:n floatin jälkeen
	glNormalPointer(GL_FLOAT, sizeof (Vertex), &vertexes_.front() + 3 * sizeof(float));

	// glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	// Tekstuurikoordinaatit structissa 6:n floattia alun jälkeen
	// glTexCoordPointer(2, GL_FLOAT, sizeof (Vertex), (char*)NULL + 6 * sizeof(float));
	// glTexCoordPointer(2, GL_FLOAT, sizeof (Vertex), &vertexes_.front() + 6 * sizeof(float));

	glDrawElements(GL_TRIANGLES, 3 * faces_.size(), GL_UNSIGNED_INT, &faces_.front());

	// glBindBuffer(GL_ARRAY_BUFFER, 0);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

