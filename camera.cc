#include <iostream>
using std::cout;
using std::endl;

#include <GL3/gl3w.h>
#include <GL/glfw.h>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common.hh"

#include "debug.hh"
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

void Camera::applyMovement(const glm::vec3& move) {
    if (move.length() > 0) {
        // glm::vec3 move2 = glm::normalize(move);
        float time2 = glfwGetTime();
        float elapsed = time2 - time_;
        time_ = time2;
        glm::vec3 move2 = move;
        move2 *= elapsed * 200.0;;
        pos_ += move2;
    }
    delta_ = glm::vec3(0);
}

glm::vec3 Camera::getMovement() const {
    return delta_;
}

glm::vec3 Camera::getPos() const {
    return pos_;
}

glm::vec3 Camera::getVector() const {
    glm::vec4 suunta(0.0, 0.0, 1.0, 0.0);
    suunta = suunta * rot_;
    return suunta.xyz;
}

void Camera::move(float amount) {
    delta_ += glm::vec3(((glm::vec4(0.0, 0.0, -1.0, 0.0) * rot_) * amount).xyz);
}

void Camera::strafe(float amount) {
    delta_ += glm::vec3(((glm::vec4(1.0, 0.0, 0.0, 0.0) * rot_) * amount).xyz);
}

void Camera::moveHeight(float amount) {
    delta_ += glm::vec3(((glm::vec4(0.0, 1.0, 0.0, 0.0) * rot_) * amount).xyz);
}

void Camera::rotate(float pitch, float heading) {
    pitch_ += pitch;
    pitch_ = glm::clamp(pitch_, -90.0f, 90.0f);
    heading_ -= heading;
    heading_ = glm::mod(heading_, 360.0f);
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
