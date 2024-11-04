#include "LinearOctree.h"
#include "ColliderObject.h"

uint32_t LinearOctree::Part1By2(uint32_t num)
{
	num = (num ^ (num << 16)) & 0xff0000ff;
	num = (num ^ (num << 8)) & 0x0300f00f;
	num = (num ^ (num << 4)) & 0x030c30c3;
	num = (num ^ (num << 2)) & 0x09249249;
	return num;
}

uint32_t LinearOctree::GetKey(const Vec3 pos) const
{
	uint32_t xIndex = (pos.x - m_min.x) / m_minExtent;
	uint32_t yIndex = (pos.y - m_min.y) / m_minExtent;
	uint32_t zIndex = (pos.z - m_min.z) / m_minExtent;
	uint32_t key = Part1By2(xIndex) + (Part1By2(yIndex) << 1) + (Part1By2(zIndex) << 2);
	key = m_rowCount + (key << 5);
}

uint32_t LinearOctree::GetKey(const ColliderObject* object) const
{
	const Vec3 max = object->position + object->size;
	const Vec3 min = object->position - object->size;

	uint32_t minKey = GetKey(min);
	uint32_t maxKey = GetKey(max);
	if (minKey == maxKey)
	{
		return minKey;
	}
	else
	{
		// calculate node that is coparent of both keys
	}
}

LinearOctree::LinearOctree(const Vec3 position, const float extent, const size_t rowCount) :
	m_extent(extent),
	m_minExtent(extent / (1 << (rowCount - 1))),
	m_rowCount(rowCount),
	m_min(position.x - extent, position.y - extent, position.z - extent),
	m_max(position.x + extent, position.y + extent, position.z + extent)
{
	for (size_t row = 1; row <= rowCount; ++row)
	{

	}
}

LinearOctree::~LinearOctree()
{
}

void LinearOctree::Insert(ColliderObject* pObj)
{
	uint32_t key = GetKey(pObj);
}

void LinearOctree::TestCollisions()
{
}

void LinearOctree::ClearLists()
{
}
