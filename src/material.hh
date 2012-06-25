#ifndef MATERIAL_HH
#define MATERIAL_HH

class Material {
public:
	Material() {};
	virtual ~Material() {};
	virtual void use() = 0;
private:
};

#endif
