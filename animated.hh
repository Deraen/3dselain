#ifndef ANIMATED_HH
#define ANIMATED_HH

class Animated {
public:
	Animated() {}

	virtual void animate(float time) = 0;
};

#endif
