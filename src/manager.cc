#include <vector>
using std::vector;
#include <map>
using std::map;
#include <string>
using std::string;

#include "manager.hh"
#include "debug.hh"

Manager::Manager():
    textures_(),
    shaders_(),
    objects_()
{}

Manager::~Manager() {
    unload();
}

Manager& Manager::instance() {
    static Manager instance_;
    return instance_;
}

void Manager::unload() {
    for (auto texture: textures_) {
        delete texture.second;
    }
    textures_.clear();

    for (auto shader: shaders_) {
        delete shader.second;
    }
    shaders_.clear();

    for (auto object: objects_) {
        delete object;
    }
    objects_.clear();
}

// --- TEXTUURIT ---
void Manager::addTexture(const string& key, const string& filename) {
    if (textures_.find(key) != textures_.end()) {
        Debug::start() << "Yritettiin lisätä tekstuuria samalla avaimella: " << key << Debug::end();
        return;
    }
    textures_[key] = new Texture(filename);
}

Texture* Manager::getTexture(const string& key) {
    auto i = textures_.find(key);
    if (i == textures_.end()) {
        Debug::start() << "Ei löydetty textuuria" << Debug::end();
        return NULL;
    }
    return i->second;
}

// --- SHADERIT ---
void Manager::addShader(const string& key) {
    if (shaders_.find(key) != shaders_.end()) {
        Debug::start() << "Yritettiin lisätä tekstuuria samalla avaimella" << Debug::end();
        return;
    }
    shaders_[key] = new Shader;
}

Shader* Manager::getShader(const string& key) {
    auto i = shaders_.find(key);
    if (i == shaders_.end()) {
        Debug::start() << "Ei löydetty shaderia" << Debug::end();
        return NULL;
    }
    return i->second;
}

// --- OBJECTS ---
void Manager::addObject(const std::string& /*key*/, Drawable* obj) {
/*
    if (objects_.find(key) != objects_.end()) {
        Debug::start() << "Yritettiin lisätä objektia samalla avaimella" << Debug::end();
        return;
    }
    objects_[key] = obj;
*/
    objects_.push_back(obj);
}

Drawable* Manager::getObject(unsigned int i) {
    if (i >= objects_.size()) return NULL;
    return objects_.at(i);
}

unsigned int Manager::objectCount() const {
    return objects_.size();
}

unsigned int Manager::findMaterial(const std::string& name) const {
    for (unsigned int i = 0; i < materials_.size(); ++i) {
        if (materials_.at(i)->name() == name) {
            return i;
        }
    }
    return Manager::NOT_FOUND;
}

unsigned int Manager::newMaterial(const std::string& name) {
    materials_.push_back(new Material(name));
    return materials_.size() - 1;
}

Material* Manager::getMaterial(unsigned int i) const {
    if (i >= materials_.size()) return NULL;
    return materials_.at(i);
}
