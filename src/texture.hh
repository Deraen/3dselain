#ifndef TEXTURE_HH
#define TEXTURE_HH

#include <GL3/gl3w.h>

#include <string>

class Texture {
public:
	Texture(const std::string& filename);
	~Texture();

	void start();
	void end();

	static bool enabled;
private:
	static GLenum textureN;

	GLenum n_;
	GLuint texture_;
};

#endif
