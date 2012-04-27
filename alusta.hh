#ifndef ALUSTA_HH
#define ALUSTA_HH

#include <vector>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "solidmaterial.hh"
#include "drawable.hh"
#include "objreader.hh"

class Alusta : public ObjReader {
public:
	Alusta();
	~Alusta();

	void draw();
private:

	SolidMaterial material_;
};

#endif
