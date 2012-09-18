#ifndef MATERIAL_HH
#define MATERIAL_HH

#include <string>

#include <glm/glm.hpp>
#include <assimp/material.h>

#include "debug.hh"

struct Material {
public:
    Material(const std::string& name);
    ~Material();

    void load(const aiMaterial* mat);
    void use() const;

    std::string name() const { return name_; }

private:
    glm::vec3 ambient_;
    glm::vec3 diffuse_;
    float opacity_;
    std::string name_;
};

#endif
