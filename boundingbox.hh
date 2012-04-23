#ifndef BOUNDINGBOX_HH
#define BOUNDINGBOX_HH

#include "vec3.hh"

struct BoundingBox {
	BoundingBox(const Vec3& pos_, const Vec3& dim_):
		pos(pos_), dim(dim_)
	{}

	~BoundingBox() {}
	bool collision(BoundingBox& box) {
		return (box.pos.x - box.dim.x / 2 < pos.x + dim.x / 2 &&
				box.pos.y - box.dim.y / 2 < pos.y + dim.y / 2 &&
				box.pos.z - box.dim.z / 2 < pos.z + dim.z / 2 &&
				box.pos.x + box.dim.x / 2 > pos.x - dim.x / 2 &&
				box.pos.y + box.dim.y / 2 > pos.y - dim.y / 2 &&
				box.pos.z + box.dim.z / 2 > pos.z - dim.z / 2);
	};

	Vec3 pos;
	Vec3 dim;
};

#endif
