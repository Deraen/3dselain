#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "alusta.hh"
#include "alusta.h" //data
#include "material.hh"

#define BUFFER_OFFSET(x)((char *)NULL+(x))

Alusta::Alusta()
{
    material_.setShinines(128);
    material_.setDiffuse(0.2, 1.0, 0.2);
    material_.setSpecular(1.0, 1.0, 1.0);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof (struct vertex_struct) * VERTEX_COUNT, vertexs, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vinx_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (indexes[0]) * FACES_COUNT * 3, indexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Alusta::~Alusta()
{}

void Alusta::draw() {
	material_.use();

	glRotatef(-90, 1.0, 0.0, 0.0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx_);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof (struct vertex_struct), BUFFER_OFFSET(0));

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, sizeof (struct vertex_struct), BUFFER_OFFSET(3 * sizeof (float)));

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof (struct vertex_struct), BUFFER_OFFSET(6 * sizeof(float)));

	glDrawElements(GL_TRIANGLES, FACES_COUNT * 3, INX_TYPE, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

