#pragma once
#include <cstdint>
#include <unordered_map>
#include "Vec3.h"

class ColliderObject;

class LinearOctree
{
	static constexpr size_t MAX_ROWS = 10;
	static constexpr uint32_t KEY_MASK = 0b111;
	static constexpr uint32_t ROW_MASK = 0b11111;

	class LinearOctant
	{
		uint32_t key;
		ColliderObject* pObjects = nullptr;
	};

	std::unordered_map<uint32_t, LinearOctant> m_octants;
	const size_t m_rowCount;
	const Vec3 m_max;
	const Vec3 m_min;
	const float m_extent;
	const float m_minExtent;

	static uint32_t Part1By2(uint32_t num);

	uint32_t GetKey(const Vec3 pos) const;
	uint32_t GetKey(const ColliderObject* object) const;

public:
	LinearOctree(const Vec3 position, const float extent, const size_t rowCount);
	~LinearOctree();

	void Insert(ColliderObject* pObj);
	void TestCollisions();
	void ClearLists();
};

