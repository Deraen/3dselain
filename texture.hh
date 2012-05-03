#ifndef TEXTURE_HH
#define TEXTURE_HH

#include <string>
#include <GL/gl.h>

#pragma pack(1)
struct TgaHeader
{
	unsigned char identSize;
	unsigned char colourMapType;
    unsigned char imageTypecode;

    unsigned short colourMapStart;
    unsigned short colourMapLength;
    unsigned char colourMapBits;

    unsigned short xorigin;
    unsigned short yorigin;
    unsigned short imageWidth;
    unsigned short imageHeight;

    unsigned char bitsPerPixel;
    unsigned char imageDescriptor;
};
#pragma pack()

const unsigned int TGAHEADER_SIZE = sizeof(TgaHeader);

union TgaUnion {
	TgaHeader data;
	char bytes[TGAHEADER_SIZE];
};

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

	char* imageData;
	TgaUnion header;
};

#endif
