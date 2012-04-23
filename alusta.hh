#ifndef ALUSTA_HH
#define ALUSTA_HH

#include <vector>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "solidmaterial.hh"
#include "drawable.hh"
#include "boundingbox.hh"
#include "objreader.hh"

class Vertex;
class Face;

class Alusta : public Drawable, public ObjReader {
public:
	Alusta();
	~Alusta();

	void draw();
	bool collision(BoundingBox& box) { return false; }
private:

	SolidMaterial material_;
};

#endif
