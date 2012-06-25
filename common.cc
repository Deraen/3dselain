#include <GL3/gl3w.h>
#include "debug.hh"

int CheckGLErrors() {
    int errCount = 0;
    for(GLenum currError = glGetError(); currError != GL_NO_ERROR; currError = glGetError()) {
        Debug::start()[0] << "OpenGL error: " << currError << Debug::end();
        ++errCount;
    }
    return errCount;
}
