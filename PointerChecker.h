#pragma once

#ifdef _DEBUG
#define Ptr(Type) Pointer<Type>

	template <typename T = void>
	class Pointer
	{
		using pointer = T*;

	private:
		pointer m_ptr = nullptr;

	public:
		Pointer() = delete;

		Pointer(pointer ptr)
		{
			m_ptr = ptr;
		}

		Pointer<T>& operator=(const Pointer<T>& other)
		{
			m_ptr = other.m_ptr;
			return *this;
		}

		operator pointer () const 
		{
			return m_ptr;
		}		

		pointer operator->() const
		{
			return m_ptr;
		}

		T& operator* () const
		{
			return *m_ptr;
		}

		Pointer<T>& operator++()
		{
			return m_ptr++;
		}

		bool operator==(const pointer other) const { return m_ptr == other; }
	};
#else
#define Ptr(Type) Type*
#endif