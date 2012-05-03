#include <string>
using std::string;
#include <fstream>
using std::ifstream;
using std::ios_base;

#include <GL/gl.h>
#include <GL/glut.h>

#include "texture.hh"
#include "debug.hh"

GLenum Texture::textureN = GL_TEXTURE0;
bool Texture::enabled = true;

Texture::Texture(const string& filename):
	n_(textureN),
	texture_(0),
    imageData(NULL),
    header()
{
	++textureN;

    // Open the TGA file.
    ifstream file(filename.c_str(), ios_base::binary);

    file.read(header.bytes, TGAHEADER_SIZE);

    if (header.data.imageTypecode != 2 || (header.data.bitsPerPixel != 24 && header.data.bitsPerPixel != 32)) {
        Debug::start()[0] << filename << ": Only support uncompressed 24- or 32-bit RGB files!" << Debug::end();
        file.close();
        return;
    }
    unsigned char bytesPerPixel = header.data.bitsPerPixel / 8;

    unsigned int fileSize = header.data.imageWidth * header.data.imageHeight * bytesPerPixel;

    imageData = new char[fileSize];
    file.read(imageData, fileSize);
    file.close();

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    // select modulate to mix texture with color for shading
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// when texture area is small, bilinear filter the closest mipmap
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	// when texture area is large, bilinear filter the original
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// the texture wraps over at the edges (repeat)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, header.data.imageWidth, header.data.imageHeight,
                      (bytesPerPixel == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE,
                      imageData );
}

Texture::~Texture() {
    delete imageData;
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
