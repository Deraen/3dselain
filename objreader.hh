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

template <class T>
inline void glBufferData(GLenum target, const std::vector<T>& v, GLenum usage) {
	glBufferData(target, v.size() * sizeof(T), &v[0], usage);
}

class ObjReader {
public:
	ObjReader(const char* filename);

	void draw();

protected:
	std::vector<Vertex> vertexes_;
	std::vector<Vec3> normals_; //temp
	std::vector<Face> faces_;
};

#endif
