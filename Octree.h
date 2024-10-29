#pragma once
#include "Vec3.h"
#include <array>
#include <mutex>
#include <atomic>

class ColliderObject;

class Octree
{
	struct Octant
	{
		// don't need to store extent as long as every object is in root node
		const Vec3 centre;
		std::array<Octant*, 8> children;

		Octant(Vec3 centre, ColliderObject* pObjects);
		void AddToList(ColliderObject* pObj);
		void TestCollisions(Octant* other);
		void ClearList();

	private:
		ColliderObject* pObjects;
		std::mutex listMutex;
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