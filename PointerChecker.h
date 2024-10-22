#pragma once
#include <exception>

#ifdef _DEBUG
#define Ptr(Type) Pointer<Type>
	/// <summary>
	/// Pointer Implementation struct to avoid void reference and increment
	/// </summary>
	template<typename T>
	struct PtrImpl
	{
		typedef T& reference;

		static reference Dereference(T* ptr)
		{
			return *ptr;
		}

		static T* Increment(T* ptr)
		{
			return ++ptr;
		}
	};

	/// <summary>
	/// Specifically declare void implementation
	/// </summary>
	template<>
	struct PtrImpl<void>
	{
		typedef void reference;

		static reference Dereference(void*)
		{
			throw std::exception("Tried to dereference void pointer");
		}

		static void* Increment(void*)
		{
			throw std::exception("Tried to increment void pointer");
		}
	};

	/// <summary>
	/// Wrapper for pointers to check for pointer issues
	/// </summary>
	template <typename T>
	class Pointer
	{
	private:
		using Impl = PtrImpl<T>;
		using reference = typename Impl::reference;
		using pointer = T*;

		pointer m_ptr = nullptr;

	public:
		Pointer()
		{
			m_ptr = nullptr;
		}

		Pointer(pointer ptr)
		{
			m_ptr = ptr;
		}

		operator pointer () const 
		{
			return m_ptr;
		}		

		pointer operator->() const
		{
			return m_ptr;
		}

		reference operator* () const
		{
			return Impl::Dereference(m_ptr);
		}

		Pointer<T>& operator++()
		{
			m_ptr = Impl::Increment(m_ptr);
			return *this;
		}

		Pointer<T> operator++(int)
		{
			auto temp = *this;
			++(*this);
			return temp;
		}

		bool operator==(const pointer other) const { return m_ptr == other; }
	};
#else
#define Ptr(Type) Type*
#endif