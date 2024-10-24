#pragma once
#include "Vec3.h"
#include <array>

class ColliderObject;

class Octree
{
	struct ObjNode
	{
		ColliderObject* pObj = nullptr;
		ObjNode* pNext = nullptr;
	};

	struct Octant
	{
		const Vec3 centre;
		const Vec3 extent;
		std::array<Octant*, 8> children;
		ObjNode* pObjects;

		Octant(Vec3 centre, Vec3 extent, ObjNode* pObjects);
	};

	Octant root;

	void InsertObject(Octant* pOctant, ColliderObject* pObj);
	void BuildTree(Octant* pCurrent, const unsigned int depth, const unsigned int maxDepth);
	void TestAllCollisions(Octant* pOctant);

	void DestroyChildren(Octant* pOctant);
	void ClearList(ObjNode* objNode);
	void ClearLists(Octant* pOctant);
public:
	Octree(const Vec3 position, const Vec3 extent, const unsigned int maxDepth);
	~Octree();

	void Insert(ColliderObject* pObj);
	void TestCollisions();
	void ResetObjects();
};