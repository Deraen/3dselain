#ifndef SUN_HH
#define SUN_HH

#include "light.hh"
#include "animated.hh"

class Sun : public Light, public Animated {
public:
	Sun();
	void draw();
	void animate(float time);
};

#endif
