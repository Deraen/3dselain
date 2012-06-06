#ifndef SHADER_HH
#define SHADER_HH

#include <string>

#include <GL3/gl3w.h>

class Shader {
public:
	Shader();
	void addVertexShader(const std::string& filename);
	void addFragmentShader(const std::string& filename);
	void addGeometryShader(const std::string& filename);
	void load();
	void unload();

	void begin();
	void end();

	GLenum handle() { return program_; }
	void bindAttrib(unsigned int i, const std::string& var);

private:
	bool loaded_;

	GLenum program_;
	GLenum vertex_;
	GLenum fragment_;
	GLenum geometry_;
};

#endif
