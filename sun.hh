#ifndef SUN_HH
#define SUN_HH

#include "light.hh"
#include "animated.hh"
#include "boundingbox.hh"

class Sun : public Light, public Animated {
public:
	Sun();
	void draw();
	void animate(float time);
	bool collision(BoundingBox& box) {
		return false;
	}
};

#endif
