#include "Octree.h"
#include "ColliderObject.h"
#include <cmath>
#include <thread>

void Octree::InsertObject(Octant* pOctant, ColliderObject* pObj)
{
	unsigned int index = 0;
	bool straddle = false;
	for (int i = 0; i < 3; i++) // each axis
	{
		// get distance between octant's centre and objects position
		// as octants centre is where the split will be on that axis
		// if the distance to the axis split is smaller than the
		// half size of the object, then the object straddles that
		// splitting axis, so stop checking other axes
		float delta = pObj->position[i] - pOctant->centre[i];
		if (abs(delta) <= pObj->size[i] / 2) 
		{
			straddle = true;
			break;
		}
		if (delta > 0.0f) index |= (1 << i); // the side the object is on affects index
	}

	// if not straddling and child exists insert deeper
	if (!straddle && pOctant->children[index])
	{
		InsertObject(pOctant->children[index], pObj);
	}
	else
	{
		// if there is no more children or the object is
		// straddling an axis, add to current octants list
		
	}
}

void Octree::BuildTree(Octant* pCurrent, const Vec3 extent, const unsigned int depth, const unsigned int maxDepth)
{
	Vec3 halfExtent = extent / 2.0f;
	Vec3 offset;

	for (unsigned int i = 0; i < 8; i++)
	{
		offset.x = ((i & 1) ? halfExtent.x : -halfExtent.x);
		offset.y = ((i & 2) ? halfExtent.y : -halfExtent.y);
		offset.z = ((i & 4) ? halfExtent.z : -halfExtent.z);
		pCurrent->children[i] = new Octant(pCurrent->centre + offset, nullptr);

		if (depth != maxDepth)
		{
			BuildTree(pCurrent->children[i], halfExtent, depth + 1, maxDepth);
		}
	}
}

void Octree::TestAllCollisions(Octant* pOctant)
{
	constexpr int MAX_DEPTH = 32;
	static Octant* ancestors[MAX_DEPTH];
	static int depth = 0;

	ancestors[depth++] = pOctant;
	for (int d = 0; d < depth; d++)
	{
		ColliderObject * objA, * objB;
		for (objA = ancestors[d]->pObjects; objA; objA = objA->pNext)
		{
			if (ancestors[d]->pObjects == pOctant->pObjects)
			{
				objB = objA->pNext;
			}
			else
			{
				objB = pOctant->pObjects;
			}

			for (; objB; objB = objB->pNext)
			{
				ColliderObject::TestCollision(objA, objB);
			}
		}
	}

	for (int i = 0; i < 8; i++)
	{
		Octant* child = pOctant->children[i];
		if (child != nullptr)
		{
			TestAllCollisions(child);
		}
	}

	depth--;
}

void Octree::DeleteChildren(Octant* pOctant)
{
	for (Octant*& child : pOctant->children)
	{
		if (child != nullptr)
		{
			DeleteChildren(child);
			delete child;
			child = nullptr;
		}
	}
}

void Octree::ClearList(Octant* pOctant)
{
	for (Octant*& child : pOctant->children)
	{
		if (child != nullptr)
		{
			ClearList(child);
		}

		pOctant->pObjects = nullptr;
	}
}

Octree::Octree(const Vec3 position, const Vec3 extent, const unsigned int maxDepth)
{
	root = new Octant(position, nullptr);
	if (maxDepth == 0) return;
	
	BuildTree(root, extent, 1, maxDepth);
}

Octree::~Octree()
{
	DeleteChildren(root);
	delete root;
}

void Octree::Insert(ColliderObject* pObj)
{
	InsertObject(root, pObj);
}

void Octree::TestCollisions()
{
	TestAllCollisions(root);
}

void Octree::ClearLists()
{
	ClearList(root);
}

Octree::Octant::Octant(Vec3 centre, ColliderObject* pObjects) : 
	centre(centre)
{
	for (Octant*& child : children)
	{
		child = nullptr;
	}
	this->pObjects = pObjects;
}

void Octree::Octant::AddToList(ColliderObject* pObj)
{
	std::lock_guard<std::mutex> lock(listMutex);
	pObj->pNext = pObjects;
	pObjects = pObj;
}
