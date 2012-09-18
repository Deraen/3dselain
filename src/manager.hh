#ifndef MANAGER_HH
#define MANAGER_HH

#include <vector>
#include <map>
#include <string>
// #include <mutex>
#include <limits>

#include "texture.hh"
#include "shader.hh"
#include "drawable.hh"
#include "material.hh"

class Manager {
public:
	static Manager& instance();

	void unload();

	void addTexture(const std::string& key, const std::string& filename);
	Texture* getTexture(const std::string& key);
	void addShader(const std::string& key);
	Shader* getShader(const std::string& key);
	void addObject(const std::string& key, Drawable* obj);
	Drawable* getObject(unsigned int i);
	unsigned int objectCount() const;

	unsigned int findMaterial(const std::string& name) const;
	unsigned int newMaterial(const std::string& name);
	Material* getMaterial(unsigned int i) const;

	Manager(const Manager&) = delete;
	void operator=(const Manager&) = delete;

	static const unsigned int NOT_FOUND = std::numeric_limits<unsigned int>::max();
private:
	Manager();
	~Manager();

	std::map<std::string, Texture*> textures_;
	std::map<std::string, Shader*> shaders_;
	std::vector<Material*> materials_;
	std::vector<Drawable*> objects_;
};

#endif
