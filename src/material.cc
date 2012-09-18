#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "scene.hh"
#include "debug.hh"
#include "common.hh"

#include "manager.hh"

#include "material.hh"


namespace {
    inline glm::vec3 color2vec(const aiColor3D& c) {
        return glm::vec3(c.r, c.g, c.b);
    }
}


Material::Material(const std::string& name):
    name_(name)
{
    Debug::start()[0] << "+++ Material: " << name_ << Debug::end();
}

Material::~Material() {
    Debug::start()[0] << "--- Material: " << name_ << Debug::end();
}



void Material::load(const aiMaterial* mat)
{
    aiColor3D color(0.0, 0.0, 0.0);
    // Material material;
    float value;

    mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    diffuse_ = color2vec(color);

    mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
    ambient_ = color2vec(color);

    // mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
    // material->setSpecular(color.r, color.g, color.b);

    mat->Get(AI_MATKEY_OPACITY, value);
    opacity_ = value;

    // mat->Get(AI_MATKEY_COLOR_EMISSIVE, color);
    // material->setEmission(color.r, color.g, color.b);

    // float shininess = 0.0;
    // mat->Get(AI_MATKEY_SHININESS_STRENGTH, shininess);
    // material->setShinines(shininess);
    return;
}

void Material::use() const {
    Shader* shader = Manager::instance().getShader("lightning"); // XXX: turha joka kierroksella hakea?
    glUniform3fv(shader->uniformLoc("in_color_diffuse"), 1, glm::value_ptr(diffuse_));
    glUniform3fv(shader->uniformLoc("in_color_ambient"), 1, glm::value_ptr(ambient_));
    glUniform1f(shader->uniformLoc("in_color_opacity"), opacity_);
    // glVertexAttrib3fv(shader->attribLoc("in_color_specular"), material->specular());
}
