#ifndef ALUSTA_HH
#define ALUSTA_HH

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "drawable.hh"
class Material;

#define BUFFER_OFFSET(x)((char *)NULL+(x))

class Alusta : public Drawable {
public:
	Alusta(Material* material);

	void draw();
private:
	GLuint vbo_;
	GLuint vinx_;
	Material* material_;
};

#endif
