#include <string>
using std::string;
#include <fstream>
using std::ifstream;
#include <map>
using std::map;

#include "common.hh"
#include "shader.hh"
#include "debug.hh"

#include <GL3/gl3w.h>

namespace {
	char* loadFile(const char* fname) {
		ifstream::pos_type size;
		char * memblock;
		string text;

		// file read based on example in cplusplus.com tutorial
		ifstream file (fname, std::ios::in | std::ios::binary | std::ios::ate);
		if (file.is_open()) {
			size = file.tellg();
			memblock = new char [(int)size + 1];
			file.seekg (0, std::ios::beg);
			file.read (memblock, size);
			file.close();
			memblock[size] = '\0';
			Debug::start()[2] << "Shader " << fname << " loaded" << Debug::end();
			text.assign(memblock);
		} else {
			Debug::start()[2] << "Unable to open file " << fname << Debug::end();
			return NULL;
		}

		return memblock;
	}

	void printShaderInfoLog(GLint shader) {
		int infoLogLen = 0;
		int charsWritten = 0;
		GLchar *infoLog;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

		// should additionally check for OpenGL errors here

		if (infoLogLen > 0) {
			infoLog = new GLchar[infoLogLen];
			// error check for fail to allocate memory omitted
			glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
			Debug::start()[1] << infoLog << Debug::end();
			delete [] infoLog;
		}

		// should additionally check for OpenGL errors here
	}

	void loadShader(const std::string& filename, GLenum program, GLenum& handle, GLenum type) {
		const char* source = loadFile(filename.c_str());
		if (source == NULL) {
			throw FatalException("Erästä shaderia ei voitu ladata.");
		}

		handle = glCreateShader(type);
		glShaderSource(handle, 1, &source, NULL); // NULL = null terminated string
		glCompileShader(handle);

		GLint compiled;
		glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

		if (!compiled) {
			Debug::start()[1] << "Shader failed to build" << Debug::end();
			printShaderInfoLog(handle);
		} else {
			glAttachShader(program, handle);
		}

		delete source;
	}
}

Shader::Shader():
	loaded_(false),
	program_(0),
	vertex_(0),
	fragment_(0),
	geometry_(0)
{
	program_ = glCreateProgram();
}

void Shader::addFragmentShader(const std::string &filename) {
	loadShader(filename, program_, fragment_, GL_FRAGMENT_SHADER);
}

void Shader::addGeometryShader(const std::string &filename) {
	loadShader(filename, program_, geometry_, GL_GEOMETRY_SHADER);
}

void Shader::addVertexShader(const std::string &filename) {
	loadShader(filename, program_, vertex_, GL_VERTEX_SHADER);
}


void Shader::load() {
	loaded_ = true;
	Debug::start()[2] << "Linking shader " << Debug::end();

	glLinkProgram(program_);
}

void Shader::unload() {
	glDeleteProgram(program_);
	glDeleteShader(vertex_);
	glDeleteShader(geometry_);
	glDeleteShader(fragment_);
	loaded_ = false;
}

void Shader::begin() {
	glUseProgram(program_);
}


void Shader::end() {
	glUseProgram(0);
}

GLuint Shader::uniformLoc(const std::string &key) {
	if (!loaded_) return -1;

	auto i = uniforms_.find(key);
	if (i != uniforms_.end()) {
		return i->second;
	}

	GLuint loc = glGetUniformLocation(program_, key.c_str());
	uniforms_[key] = loc;
	return loc;
}


GLuint Shader::attribLoc(const std::string &key) {
	if (!loaded_) return -1;

	auto i = attribs_.find(key);
	if (i != attribs_.end()) {
		return i->second;
	}

	GLuint loc = glGetAttribLocation(program_, key.c_str());
	attribs_[key] = loc;
	return loc;
}
