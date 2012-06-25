#ifndef BLOCK_HH
#define BLOCK_HH

#include "scene.hh"

class Block : public Scene {
public:
	Block(const std::string& filename, const glm::vec3& origin, const glm::vec3& bounding_box_size);
	void visibility(const glm::vec3& pos);
private:
    glm::vec3 center_;
    glm::vec3 min_;
    glm::vec3 max_;
};

#endif
