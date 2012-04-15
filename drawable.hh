#ifndef DRAWABLE_HH
#define DRAWABLE_HH

class Drawable {
public:
	Drawable() {};
	virtual void draw() = 0;
	virtual ~Drawable() {}

private:
};

#endif
