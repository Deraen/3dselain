#include <string>
using std::string;
#include <fstream>
using std::ifstream;
using std::ios_base;

#include <GL/gl.h>
#include <GL/glfw.h>

#include "texture.hh"
#include "debug.hh"

GLenum Texture::textureN = GL_TEXTURE0;
bool Texture::enabled = true;

Texture::Texture(const string& filename):
	n_(textureN),
	texture_(0)
{
	++textureN;

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);

    glfwLoadTexture2D(filename.c_str(), GLFW_BUILD_MIPMAPS_BIT);


    // select modulate to mix texture with color for shading
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// when texture area is small, bilinear filter the closest mipmap
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	// when texture area is large, bilinear filter the original
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// the texture wraps over at the edges (repeat)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Texture::~Texture() {
}


void Texture::start() {
	if (enabled) {
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, texture_ );
	}
}

void Texture::end() {
	glDisable(GL_TEXTURE_2D);
}
