#ifndef LOCK_H
#define LOCK_H



#ifndef NO_NAMESPACES
namespace util_thread
{
#endif



template <class T>
class Lock
{
	public:
		Lock(T& t) : t_(t), locked_(false) { t_.acquire(); locked_ = true;}
		~Lock() { if ( locked_ ) t_.release(); }

		bool release() { bool tmp = locked_; locked_ = false; return tmp; }

	private:
		T& t_;
		bool locked_;

		// Prevent assignment operator
		Lock<T>& operator = (const Lock<T>& rhs);
};



#ifndef NO_NAMESPACES
} // namespace util_thread
#endif



#endif // LOCK_H
