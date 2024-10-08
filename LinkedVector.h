#pragma once
#include <vector>

/// <summary>
/// Container to wrap around vector to allow linked list of same type vectors that can iterate over all elements in all vectors
/// </summary>
template <class T>
class LinkedVector
{
public:
	/// <summary>
	/// Iterator struct for use in ranged base for loop
	/// </summary>
	struct Iterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;

		Iterator(LinkedVector<T>* vector) 
		{
			ptr = vector->beginPtr(); 
			linkedVec = vector; 
			endPtr = vector->endPtr(); 
		}

		Iterator(pointer ptr) { this->ptr = ptr; linkedVec = nullptr; endPtr = nullptr; } // for creating end iterator
		Iterator(const LinkedVector<T>::Iterator& rawIterator) = default; // copy constructor
		~Iterator() {}

		// accessing current iterator pointer
		reference operator*() const { return *ptr; }
		pointer operator->() { return ptr; }

		// operators to increment the iterator
		Iterator& operator++() 
		{ 
			ptr++;
			if (ptr == endPtr && linkedVec->next != nullptr)
			{
				linkedVec = linkedVec->next;
				ptr = linkedVec->beginPtr();
				endPtr = linkedVec->endPtr();
			}

			return *this;
		}

		Iterator operator++(int) 
		{ 
			Iterator tmp = *this;
			++(*this);
			return tmp; 
		}

		// checking iterator equality
		friend bool operator== (const Iterator& left, const Iterator& right) { return left.ptr == right.ptr; }
		friend bool operator!= (const Iterator& left, const Iterator& right) { return left.ptr != right.ptr; }

	private:
		pointer ptr; 
		pointer endPtr; // end pointer of current vector
		LinkedVector<T>* linkedVec; // current vector (in linked vector container)
	};

private:
	LinkedVector* next;

	/// <summary>
	/// returns stored vectors begin
	/// </summary>
	inline T* beginPtr() noexcept
	{
		return vector.data();
	}

	/// <summary>
	/// returns stored vectors end
	/// </summary>
	inline T* endPtr() noexcept
	{
		return &vector.data()[vector.size()];
	}

public:
	std::vector<T> vector; 

	LinkedVector(const unsigned int size = 0) : vector{size}
	{
		next = nullptr;
	}
	LinkedVector(LinkedVector* pNext, const unsigned int size = 0) : vector{ size }
	{
		next = pNext;
	}

	inline T& operator[](const unsigned int _Pos) noexcept
	{
		return vector[_Pos];
	}

	inline Iterator begin() noexcept 
	{
		if (vector.size() == 0 && next != nullptr)
		{
			return next->begin();
		}
		else
		{
			return Iterator(this);
		}
	}

	inline Iterator end() noexcept 
	{
		LinkedVector* end = this;
		while (end->next != nullptr) { end = end->next; }

		return Iterator(end->endPtr());
	}

	void clear()
	{
		LinkedVector* linkedVector = this;
		while (linkedVector != nullptr)
		{
			linkedVector->vector.clear();
			linkedVector = linkedVector->next;
		}
	}
};