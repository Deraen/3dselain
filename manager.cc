#include <vector>
using std::vector;
#include <map>
using std::map;
#include <string>
using std::string;

#include "manager.hh"
#include "debug.hh"

Manager* Manager::instance_ = NULL;

Manager::Manager():
    textures_(),
    shaders_(),
    objects_()
{}

Manager::~Manager() {
    for (map<string, Texture*>::iterator i = textures_.begin();
         i != textures_.end(); ++i) {
        delete i->second;
    }

    for (map<string, Shader*>::iterator i = shaders_.begin();
         i != shaders_.end(); ++i) {
        delete i->second;
    }

    for (unsigned int i = 0; i < objects_.size(); ++i) {
        delete objects_.at(i);
    }
}

Manager& Manager::instance() {
    if (!instance_) {
        instance_ = new Manager;
    }
    return *instance_;
}

// --- TEXTUURIT ---
void Manager::addTexture(const string& key, const string& filename) {
    if (textures_.find(key) != textures_.end()) {
        Debug::start() << "Yritettiin lisätä tekstuuria samalla avaimella" << Debug::end();
        return;
    }
    textures_[key] = new Texture(filename);
}

Texture* Manager::getTexture(const string& key) {
    map<string, Texture*>::iterator i = textures_.find(key);
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
    map<string, Shader*>::iterator i = shaders_.find(key);
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
