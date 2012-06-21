#ifndef CAMERA_HH
#define CAMERA_HH

#include <glm/glm.hpp>
// glm::value_ptr
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "common.hh"

class Camera {
public:
    Camera(const float x, const float y, const float z);

    void move(float amount);

    void strafe(float amount);

    void moveHeight(float amount);

    void rotate(float pitch, float heading);
    // void pitch(float amount);

    // void heading(float amount);

    glm::vec3 getMovement() const;

    void applyMovement(const glm::vec3& move);

    void setup();
    // float* modelview();
    // float* projection(unsigned int w, unsigned int h);
    // float* location();

    glm::vec3 getPos() const;
    // Vec3 kameran katsomis suuntaan
    glm::vec3 getVector() const;

private:
    glm::vec3 pos_;
    glm::vec3 delta_;
    glm::mat4 rot_;
    // glm::quat rot_;
    float pitch_;
    float heading_;
    float time_;
    // glm::mat4 projection_;
    // glm::mat4 location_;
};

#endif
