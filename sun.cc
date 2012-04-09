#include <cmath>

#include "sun.hh"

Sun::Sun(GLenum n): Light(n)
{
	setSpecular(1.0, 1.0, 1.0);
    setAmbient(0.0, 0.0, 0.0);
    setDiffuse(1.0, 1.0, 1.0);
}

void Sun::draw() {
	glLightf(n, GL_LINEAR_ATTENUATION, 0.01); // Valo heikkenee hieman etäisyyden funktiona
	glLightf(n, GL_QUADRATIC_ATTENUATION, 0.01);

	Light::draw();
}

void Sun::animate(float time) {
    setPos(10 * sin(time), 25.0, 10 * cos(time));
}
