#include "Octree.h"
#include <cmath>

Octree::Octree(Vec3 position, Vec3 extent, Octree* parent, unsigned int count) :
	position(position),
	extent(extent),
	parent(parent)
{
	if (count == 0)
	{
		for (int i = 0; i < 8; i++)
		{
			children[i] = nullptr;
		}
		return;
	}

	Octree* givenParent = this;
	if (count == 1) givenParent = nullptr;

	count--;
	Vec3 newExtent = extent / 2.0f; // element wise division not length
	Vec3 newPositions[8] = {
		position - newExtent,
		position - Vec3{newExtent.x, newExtent.y, -newExtent.z},
		position - Vec3{-newExtent.x, newExtent.y, newExtent.z},
		position - Vec3{-newExtent.x, newExtent.y, -newExtent.z},
		position + Vec3{newExtent.x, newExtent.y, -newExtent.z},
		position + Vec3{-newExtent.x, newExtent.y, newExtent.z},
		position + Vec3{-newExtent.x, newExtent.y, -newExtent.z},
		position + newExtent
	};
	for (int i = 0; i < 8; i++)
	{
		children[i] = new Octree(newPositions[i], newExtent, givenParent, count);
	}
}