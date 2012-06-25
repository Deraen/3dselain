#ifndef FACE_HH
#define FACE_HH

struct Face {
    glm::vec3* a;
    glm::vec3* b;
    glm::vec3* c;
    glm::vec3 normal;
    float d;
    glm::vec3 min;
    glm::vec3 max;

    Face(glm::vec3* a_, glm::vec3* b_, glm::vec3* c_);
    bool isPointInside(const glm::vec3& point) const;
};

#endif
