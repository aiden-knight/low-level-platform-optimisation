#pragma once
#include <vector>

template <class T>
class LinkedVector
{
public:
	struct Iterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;

		Iterator(pointer ptr = nullptr) { this->ptr = ptr; }
		Iterator(const LinkedVector<T>::Iterator& rawIterator) = default; // copy constructor
		~Iterator() {}

		reference operator*() const { return *ptr; }
		pointer operator->() { return ptr; }

		Iterator& operator++() { ptr++; return *this; }
		Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

		friend bool operator== (const Iterator& left, const Iterator& right) { return left.ptr == right.ptr; }
		friend bool operator!= (const Iterator& left, const Iterator& right) { return left.ptr != right.ptr; }

	private:
		pointer ptr;
	};

private:
	std::vector<T> vector;
	LinkedVector* next;

public:
	LinkedVector(const unsigned int size) : vector{size}
	{
		next = nullptr;
	}

	inline T& operator[](const unsigned int _Pos) noexcept /* strengthened */ {
		return vector[_Pos];
	}

	inline Iterator begin() noexcept {
		return Iterator(&vector.data()[0]);
	}

	inline Iterator end() noexcept {
		return Iterator(&vector.data()[vector.size()]);
	}

	T* data() { return vector.data(); }
	size_t size() const { return vector.size(); }
};