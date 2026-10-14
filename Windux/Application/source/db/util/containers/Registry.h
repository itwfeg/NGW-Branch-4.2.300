#ifndef REGISTRY_H
#define REGISTRY_H



#include <string>
#include <set>
#include <ctime>
#include <functional>
#include <algorithm>
#if defined(LINUX_FIX)
#include <iostream>
#include <iterator>
#endif

#ifdef WIN32_OS
#include "..\thread\mutex.h"
#include "..\thread\lock.h"
#endif
//#ifdef VXWORKS
#if defined( __linux__ ) || defined( VXWORKS )
#include "../thread/mutex.h"
#include "../thread/lock.h"
#endif


#ifndef NO_NAMESPACES
namespace util_registry
{
#endif


	// A Registry is a thread-safe set of Entries of type T
	template <class T>
	class Registry
	{
		public:
			// Types, subtypes, and friends
			class Entry
			{
				friend class Registry<T>;

				public:
					Entry(T* t) : object(t)/*, secs(std::time(0))*/ {}
					~Entry() {}

					bool operator<(const Entry& e2) const { return this->object < e2.object; }
					operator T*() const { return (this->object); }

				private:
					T* object;
//					std::time_t secs;
			};
			typedef std::set<Entry> container_type;
			typedef typename container_type::size_type size_type;
			typedef typename container_type::iterator iterator;
			typedef typename container_type::const_iterator const_iterator;
			typedef typename container_type::reverse_iterator reverse_iterator;
			typedef typename container_type::const_reverse_iterator const_reverse_iterator;
/*
			typedef T value_type;
			typedef T* pointer;
			typedef const T* const_pointer;
			typedef T& reference;
			typedef const T& const_reference;
  */

			// Constructors and destructor
			Registry() {}
			virtual ~Registry() {}


			// Getter and setter methods


			// Processing methods
			bool add(T* t);
			bool remove(T* t);
			void removeAll();
/*
			iterator begin() { return registered.begin(); }
			const_iterator begin() const { return registered.begin(); }
			iterator end() { return registered.end(); }
			const_iterator end() const { return registered.end(); }

			iterator rbegin() { return registered.rbegin(); }
			const_iterator rbegin() const { return registered.rbegin(); }
			iterator rend() { return registered.rend(); }
			const_iterator rend() const { return registered.rend(); }
  */
			size_type size() const { util_thread::Lock< util_thread::Mutex<> > lock(mutex_); return registered_.size(); }


			// add push_back (add) and erase (remove) in STD container style


			template <class R> void process(R (*p)(T*))
					{ util_thread::Lock< util_thread::Mutex<> > lock(mutex_); for_each(registered_.begin(), registered_.end(), p); }
			template <class R> void process_mem(R (T::*p)())
					{ util_thread::Lock< util_thread::Mutex<> > lock(mutex_); for_each(registered_.begin(), registered_.end(), std::mem_fun(p)); }


			friend std::ostream& operator<<(std::ostream& os, const Registry& reg)
					{
                    #if defined( WIN32 )
                        util_thread::Lock< util_thread::Mutex<> > lock(reg.mutex_); copy(reg.registered_.begin(), reg.registered_.end(), ostream_iterator<T>(cout, "\n"));
                    #elif   defined( __linux__ )
                        util_thread::Lock< util_thread::Mutex<> > lock(reg.mutex_); copy(reg.registered_.begin(), reg.registered_.end(), std::ostream_iterator<T>(std::cout, "\n"));
                        return os;
                    #else
                        #error "Unsupported system"
                    #endif
                    }


		private:
			container_type registered_;
			mutable util_thread::Mutex<> mutex_;
	};


	template <class T>
	bool Registry<T>::add(T* t)
	{
		util_thread::Lock< util_thread::Mutex<> > lock(mutex_);

    #if defined(LINUX_FIX)
        const_iterator iter = registered_.find(Entry(t));
    #else
		container_type::const_iterator iter = registered_.find(Entry(t));
    #endif

		if ( iter == registered_.end() )
		{
			registered_.insert(Entry(t));

			return true;
		}
		else
			return false;
	}


	template <class T>
	bool Registry<T>::remove(T* t)
	{
		util_thread::Lock< util_thread::Mutex<> > lock(mutex_);

		const_iterator iter = registered_.find(Entry(t));

		if ( iter != registered_.end() )
		{
			registered_.erase(*iter);

			return true;
		}
		else
			return false;
	}


	template <class T>
	void Registry<T>::removeAll()
	{
		util_thread::Lock< util_thread::Mutex<> > lock(mutex_);

		registered_.clear();
	}


#ifndef NO_NAMESPACES
} // namespace util_registry
#endif



#endif // REGISTRY_H

