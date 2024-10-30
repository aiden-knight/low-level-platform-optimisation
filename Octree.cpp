#include "Octree.h"
#include "ColliderObject.h"
#include <cmath>
#include <thread>
#include <iostream>
#include "ThreadPool.h"
#include "MemoryOperators.h"
#include "TrackerIndex.h"

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
		pOctant->AddToList(pObj);
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
		Octant* pOther = ancestors[d];
		pOctant->TestCollisions(pOther);
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

		pOctant->ClearList();
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

#ifdef _DEBUG
void* Octree::Octant::operator new(size_t size)
{
	return ::operator new(size, MemoryManager::TrackerIndex::Octant);
}
#endif

Octree::Octant::Octant(Vec3 centre, ColliderObject* pObjects) :
	centre(centre)
{
	children = std::array<Octant*, 8>();
	for (Octant*& child : children)
	{
		child = nullptr;
	}
	this->pObjects = pObjects;
}

void Octree::Octant::AddToList(ColliderObject* pObj)
{
	std::lock_guard<std::mutex> guard(listMutex);
	pObj->pNext = pObjects;
	pObjects = pObj;
}

void Octree::Octant::TestCollisions(Octant* other)
{
	std::lock_guard<std::mutex> guard(listMutex);
	ColliderObject* objA, *objB;
	if (this != other)
	{
		std::lock_guard<std::mutex> otherGuard(other->listMutex);

		for (objA = other->pObjects; objA; objA = objA->pNext)
		{
			for (objB = pObjects; objB; objB = objB->pNext)
			{
				ColliderObject::TestCollision(objA, objB);
			}
		}
	}
	else
	{
		for (objA = other->pObjects; objA; objA = objA->pNext)
		{
			for (objB = objA->pNext; objB; objB = objB->pNext) // only check each once if is same octant
			{
				ColliderObject::TestCollision(objA, objB);
			}
		}
	}
}

void Octree::Octant::ClearList()
{
	std::lock_guard<std::mutex> guard(listMutex);
	pObjects = nullptr;
}