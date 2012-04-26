#ifndef OBJREADER_HH
#define OBJREADER_HH

#include <string>
#include <vector>

#include <GL/gl.h>

#include "vec3.hh"

struct Vertex {
	float x, y, z;
	float nx, ny, nz;

	Vertex(float x_, float y_, float z_):
		x(x_), y(y_), z(z_),
		nx(0.0), ny(0.0), nz(0.0)
	{}
};

struct Face {
	unsigned int a;
	unsigned int b;
	unsigned int c;

	Face(unsigned int a_, unsigned int b_, unsigned int c_):
		a(a_), b(b_), c(c_)
	{}
};

struct VertexEstimate {
	struct Faces {
		// Yksi oikeastaan turha
		Vertex* a;
		Vertex* b;
		Vertex* c;
		Faces* next;
	};

	Vertex* point;
	Faces* faces;

	VertexEstimate(Vertex* point_):
		point(point_),
		faces(NULL)
	{}
	void appendFace(Vertex* a_, Vertex* b_, Vertex* c_);
};

class ObjReader {
public:
	ObjReader(const char* filename);

	void draw();

protected:
	std::vector<Vertex> vertexes_;
	std::vector<Face> faces_;

	VertexEstimate* tree_;

	GLuint vbo_;
	GLuint vinx_;
};

#endif
