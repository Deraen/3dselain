#ifndef SUN_HH
#define SUN_HH

#include "light.hh"
#include "animated.hh"

class Sun : public Light, public Animated {
public:
	Sun(GLenum n);
	void draw();
	void animate(float time);
};

#endif
