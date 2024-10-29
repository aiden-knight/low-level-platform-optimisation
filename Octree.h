#pragma once
#include "Vec3.h"
#include <array>
#include <mutex>

class ColliderObject;

class Octree
{
	struct Octant
	{
		// don't need to store extent as long as every object is in root node
		const Vec3 centre;
		std::array<Octant*, 8> children;
		ColliderObject* pObjects;
		std::mutex listMutex;

		Octant(Vec3 centre, ColliderObject* pObjects);

		void AddToList(ColliderObject* pObj);
	};

	Octant* root;

	void InsertObject(Octant* pOctant, ColliderObject* pObj);
	void BuildTree(Octant* pCurrent, const Vec3 extent, const unsigned int depth, const unsigned int maxDepth);
	void TestAllCollisions(Octant* pOctant);

	void DeleteChildren(Octant* pOctant);
	void ClearList(Octant* pOctant);

public:
	Octree(const Vec3 position, const Vec3 extent, const unsigned int maxDepth);
	~Octree();

	void Insert(ColliderObject* pObj);
	void TestCollisions();
	void ClearLists();
};