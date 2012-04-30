#ifndef TEXTURE_HH
#define TEXTURE_HH

#include <string>
#include <GL/gl.h>

class Texture {
public:
	Texture(const std::string& filename);

	void start();
	void end();

	static bool enabled;
private:
	static GLenum textureN;

	GLenum n_;
	GLuint texture_;

	struct TgaFile
	{
	    unsigned char imageTypeCode;
	    short int imageWidth;
	    short int imageHeight;
	    unsigned char bitCount;
	    unsigned char *imageData;
	};
};

#endif
