#ifndef PTR_H
#define PTR_H


#include <cstddef>



template<class T> T* clone(const T* tp)
{
	return new T(*tp);
}


template <class T>
class Ptr
{
	public:
		Ptr() : pointer(0), counter(new size_t(1)) {}
		Ptr(T* t) : pointer(t), counter(new size_t(1)) {}
		Ptr(const Ptr& p) : pointer(p.pointer), counter(p.counter) { ++*counter; }
		virtual ~Ptr();

		Ptr& operator=(const Ptr& rhs);
		operator bool() const { return pointer; }
		const T& operator*() const;
		const T* operator->() const;
		T& operator*();
		T* operator->();

		void make_unique();

	private:
		T* pointer;
		std::size_t* counter;
};


template<class T> Ptr<T>::~Ptr()
{
	if ( --*counter == 0 )
	{
		delete counter;
		delete pointer;
	}
}


template<class T> Ptr<T>& Ptr<T>::operator=(const Ptr& rhs)
{
	// Prevent self-assignment
	if ( this == &rhs )
		return *this;

	// Deal with last reference case
	if ( --*counter == 0 )
	{
		delete pointer;
		pointer = 0;
	}

	// Now copy the rhs' elements onto the lhs.
	if ( rhs.pointer )
		pointer = rhs.pointer->clone();
	*counter = 1;

	return *this;
}


template<class T> const T& Ptr<T>::operator*() const
{
	if ( pointer )
		return *pointer;

	throw std::runtime_error("Ptr: unbound pointer");
}


template<class T> const T* Ptr<T>::operator->() const
{
	if ( pointer )
		return pointer;

	throw std::runtime_error("Ptr: unbound pointer");
}


template<class T> T& Ptr<T>::operator*()
{
	if ( pointer )
		return *pointer;

	throw std::runtime_error("Ptr: unbound pointer");
}


template<class T> T* Ptr<T>::operator->()
{
	if ( pointer )
		return pointer;

	throw std::runtime_error("Ptr: unbound pointer");
}


template<class T> void Ptr<T>::make_unique()
{
	if ( *counter != 1 )
	{
		--*counter;
		counter = new size_t(1);
		pointer = pointer ? clone(pointer) : 0;
	}
}



#endif // PTR_H
