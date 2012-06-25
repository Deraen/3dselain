#ifndef CAMERA_HH
#define CAMERA_HH

#include <mutex>

#include <glm/glm.hpp>
// glm::value_ptr

#include "common.hh"

class Camera {
public:
    Camera(const float x, const float y, const float z);

    void move(float amount);
    void strafe(float amount);
    void moveHeight(float amount);

    void rotate(float pitch, float heading);

    glm::vec3 getMovement() const;
    void applyMovement(const glm::vec3& move);

    void setup();
    glm::vec3 getPos() const;
    // Suuntavektori eteepäin kameran suuntaan
    glm::vec3 getVector() const;

private:
    glm::vec3 pos_;
    glm::vec3 delta_;

    // Tähän lasketaan joka pyörityksen jälkeen uusi pyöritys
    glm::mat4 rot_;

    float pitch_;
    float heading_;

    // Edellisen liikkumisen ajanhetki, normalisoidaan liikkumisnopeus
    float time_;

    std::mutex mutex_;
};

#endif
