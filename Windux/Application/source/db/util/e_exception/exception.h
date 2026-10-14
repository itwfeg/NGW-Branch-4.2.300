#ifndef EXCEPTION_H
#define EXCEPTION_H



#include <stdexcept>
#include <string>


#ifndef NO_NAMESPACES
namespace util_exception
{
#endif


#ifndef NO_NAMESPACES
	class Exception : public std::runtime_error
#else
	class Exception : public runtime_error
#endif
	{
		public:
			// Types, subtypes, and friends

			// Constructors and destructor
			Exception(const std::string& what, const std::exception* e = 0);
			virtual ~Exception() throw();

			// Getter and setter methods
			const std::exception* next() const { return ex; }

			// Processing methods
			const std::string& info() const;


		private:
			const std::exception* ex;
			mutable std::string message;
	};


#ifndef NO_NAMESPACES
} // namespace util_exception
#endif



#endif // EXCEPTION_H

