#pragma once
#include <memory>

template<typename T>
class CustomAllocator
{
public:
	using value_type = T;

	CustomAllocator() = default;

	// Copy constructor for the allocator for copy between different allocator types
	template<typename U>
	constexpr CustomAllocator(const CustomAllocator<U>&) noexcept {}

	/// <summary>
	/// Allocates memory for number of elements
	/// </summary>
	/// <param name="number">the number of elements to allocate for</param>
	/// <returns>Pointer to memory</returns>
	T* allocate(std::size_t number)
	{
		if (number > std::allocator_traits<CustomAllocator>::max_size(*this))
		{
			throw std::bad_alloc();
		}
		return static_cast<T*>(::operator new(number * sizeof(T)));
	}

	/// <summary>
	/// Deallocates memory
	/// </summary>
	/// <param name="ptr">pointer that gets delete called on</param>
	void deallocate(T* ptr, std::size_t number) noexcept
	{
		::operator delete(ptr);
	}
	
	/// <summary>
	/// Constructs object in place using placement new and arguments given
	/// </summary>
	/// <typeparam name="U">Type of object to construct</typeparam>
	/// <param name="ptr">Pointer of location to construct object</param>
	template<typename U, typename... Args>
	void construct(U* ptr, Args&&... args)
	{
		new(ptr) U(std::forward<Args>(args)...);
	}

	/// <summary>
	/// Calls objects destructor
	/// </summary>
	/// <typeparam name="U">Type of object to deconstruct</typeparam>
	/// <param name="ptr">Pointer to object</param>
	template<typename U>
	void destroy(U* ptr) noexcept
	{
		ptr->~U();
	}

	// Comparison operators required for standard library
	friend bool operator==(const CustomAllocator&, const CustomAllocator&) { return true; }
	friend bool operator!=(const CustomAllocator&, const CustomAllocator&) { return false; }
};

