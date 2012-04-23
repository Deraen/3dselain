#ifndef DRAWABLE_HH
#define DRAWABLE_HH

class BoundingBox;

class Drawable {
public:
	Drawable() {};
	virtual void draw() = 0;
	virtual ~Drawable() {}
	virtual bool collision(BoundingBox& box) = 0;

private:
};

#endif
