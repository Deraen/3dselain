#ifndef DRAWABLE_HH
#define DRAWABLE_HH

#include <glm/glm.hpp>

class Drawable {
public:
	Drawable() {};
	virtual void draw() const = 0;
	virtual ~Drawable() {}
	virtual bool collision(const glm::vec3& point, glm::vec3& movement) const = 0;

private:
};

#endif
