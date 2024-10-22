#pragma once
#include "Vec3.h"
#include <array>
#include <vector>

class ColliderObject;

class Octree
{
	const Vec3 position;
	const Vec3 extent;
	const Octree* parent;

	std::array<Octree*, 8> children;
	std::vector<ColliderObject*> colliders;

public:
	Octree(Vec3 position, Vec3 extent, Octree* parent, unsigned int count);
};