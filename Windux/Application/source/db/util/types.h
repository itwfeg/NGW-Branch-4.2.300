#ifndef TYPES_H
#define TYPES_H




// Include ptr.h for Ptrs (smart pointer)
// #include "ptr/ptr.h"




// --- Definition of 64 bit integer
#ifdef VXWORKS
typedef long long long64;
#endif
#ifdef WIN32_OS
typedef __int64 long64;
#endif

#ifdef VXWORKS
#include <ostream>
#include <iomanip>
#ifndef NO_NAMESPACES
namespace std
{
#endif
	inline ostream& operator<<(ostream& os, const long64 lng)
	{
		long high = (long)(lng/1000000000L);
		if ( high > 0 )
			return os << high << setw(9) << setfill('0') << (long)(lng%1000000000L);
		else if ( high < 0 )
			return os << high << setw(9) << setfill('0') << (long)(-lng%1000000000L);
		else
			return os << (long)lng;
	}
#ifndef NO_NAMESPACES
}
#endif
#endif // VXWORKS



// --- Definition of Flags type
struct Flags
{
	Flags(int flags = 0) : flags_(flags) {}

	bool isFlag(int flag) const { return (flags_ & flag) != 0; }   /* (SJJC port 6/19/06) */
	void setFlag(int flag) { flags_ |= flag; }

	bool isFlags(int flags) const { return (flags_ & flags) == flags; }

	int flags_;
};



#endif // TYPES_H


