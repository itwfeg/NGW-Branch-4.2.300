#ifndef UTILNIBBLE_H
#define UTILNIBBLE_H



#include <string>
#ifdef VXWORKS
#  include <stdio.h>
#else
#  include <sstream>
#endif


#ifndef NO_NAMESPACES
namespace util_nibble
{
#endif


	std::string chartohex(const char ch);
	void nibblize(const char* source, std::string& destination, int length);
	char hextochar(const char ch1, const char ch2);
	void unnibblize(const std::string& source, std::string& destination, std::string::size_type length);
	void unnibblize(const std::string& source, char* destination, int length);
	void unnibblize(const char* source, char* destination, int length);


#ifndef NO_NAMESPACES
} // namespace util_nibble
#endif



#endif // UTILNIBBLE_H

