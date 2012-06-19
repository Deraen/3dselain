#include <iostream>
using std::cout;
using std::endl;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL3/gl3w.h>
#include <GL/glfw.h>

#include "common.hh"

#include "camera.hh"
#include "shader.hh"
#include "manager.hh"

Camera::Camera(const float x, const float y, const float z):
    pos_(x, y, z),
    delta_(0.0, 0.0, 0.0),
    rot_(),
    pitch_(0.0),
    heading_(0.0)
{
    rotate(-25, 45);
}

void Camera::applyMovement(const Vec3& move) {
    pos_.x += move.x;
    pos_.y += move.y;
    pos_.z += move.z;
    delta_ = glm::vec3(0);
}

Vec3 Camera::getMovement() const {
    return Vec3(delta_.x, delta_.y, delta_.z);
}

Vec3 Camera::getPos() const {
    return Vec3(pos_.x, pos_.y, pos_.z);
}

Vec3 Camera::getVector() const {
    glm::vec4 suunta(0.0, 0.0, 1.0, 0.0);
    suunta = suunta * rot_;
    return Vec3(suunta.x, suunta.y, suunta.z);
}

void Camera::move(float amount) {
    glm::vec4 suunta(0.0, 0.0, -1.0, 0.0);
    suunta = suunta * rot_;
    suunta *= amount;
    delta_ += glm::vec3(suunta.x, suunta.y, suunta.z);
}

void Camera::strafe(float amount) {
    glm::vec4 suunta(1.0, 0.0, 0.0, 0.0);
    suunta = suunta * rot_;
    suunta *= amount;
    delta_ += glm::vec3(suunta.x, suunta.y, suunta.z);
}

void Camera::moveHeight(float amount) {
    glm::vec4 suunta(0.0, 1.0, 0.0, 0.0);
    suunta = suunta * rot_;
    suunta *= amount;
    delta_ += glm::vec3(suunta.x, suunta.y, suunta.z);
}

void Camera::rotate(float pitch, float heading) {
    pitch_ += pitch;
    if (pitch_ < -90.0) pitch_ = -90.0;
    else if (pitch_ > 90.0) pitch_ = 90.0;
    heading_ -= heading;
    heading_ = fmod(heading_, 360.0);
    rot_ = glm::rotate(glm::mat4(), pitch_, glm::vec3(-1.0, 0.0, 0.0));
    rot_ = glm::rotate(rot_, heading_, glm::vec3(0.0, 1.0, 0.0));
}

void Camera::setup() {
    int w, h;
    glfwGetWindowSize(&w, &h);
    Shader* shader = Manager::instance().getShader("lightning");

    // XXX: muuttuu vain kun ikkunan koko
    glm::mat4 projection = glm::perspective(45.0f, (1.0f * w) / h, 5.0f, 1000.0f);
    glUniformMatrix4fv(shader->uniformLoc("projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glUniformMatrix4fv(shader->uniformLoc("modelview"), 1, GL_FALSE, glm::value_ptr(rot_));

    glm::mat4 location = glm::translate(glm::mat4(1.0), -pos_);
    glUniformMatrix4fv(shader->uniformLoc("location"), 1, GL_FALSE, glm::value_ptr(location));
}
