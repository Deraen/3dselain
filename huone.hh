#ifndef HUONE_HH
#define HUONE_HH

#include <vector>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "solidmaterial.hh"
#include "drawable.hh"
#include "boundingbox.hh"
#include "objreader.hh"

class Huone : public Drawable, public ObjReader {
public:
	Huone():
		ObjReader("versio11.obj"),
		material_()
	{}
	~Huone() {};

	void draw() { ObjReader::draw(); }
	bool collision(BoundingBox& box) { return false; }
private:

	SolidMaterial material_;
};

#endif
