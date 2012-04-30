#include <cmath>
#include <GL/glut.h>

#include "sun.hh"

Sun::Sun(): Light()
{
	setSpecular(1.0, 1.0, 1.0);
    setAmbient(0.1, 0.1, 0.1);
    setDiffuse(1.0, 1.0, 1.0);
}

void Sun::draw() {
	glTranslatef(position[0], position[1], position[2]);
	glutSolidSphere(2.0, 16, 16);

	Light::draw();
}

void Sun::animate(float time) {
    setPos(10 * sin(time), 25.0, 10 * cos(time));
}
