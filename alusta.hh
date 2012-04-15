#ifndef ALUSTA_HH
#define ALUSTA_HH

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "solidmaterial.hh"
#include "drawable.hh"

#define BUFFER_OFFSET(x)((char *)NULL+(x))

class Alusta : public Drawable {
public:
	Alusta();
	~Alusta();

	void draw();
private:
	GLuint vbo_;
	GLuint vinx_;
	SolidMaterial material_;
};

#endif
