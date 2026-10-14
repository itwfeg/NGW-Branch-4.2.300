#include <stdexcept>
#include <cctype>



#include "nibble.h"



#ifndef NO_NAMESPACES
namespace util_nibble
{
#endif



	inline int chartohexint(char c)
	{
		if ( isdigit(c) )
			return c - '0';

		if ( isxdigit(c) )
			return toupper(c) - '7'; // = toupper(c) - 'A' + 10 = toupper(c) - ('A' - 10)

		throw std::out_of_range("invalid hex character");
	}


	inline char hexinttochar(int c)
	{
		if ( 0 <= c && c <= 9 ) return (char)(c + '0');
		if ( 10 <= c && c <= 15 ) return (char)(c + '7'); // = c - 10 + 'A' = c + ('A' - 10)

		throw std::out_of_range("unable to convert to hex character");
	}


	char top_nibble(unsigned char c)
	{
		return hexinttochar(c>>4);
	}


	char bottom_nibble(unsigned char c)
	{
		return hexinttochar(c&0x0F);
	}


	std::string chartohex(const char ch)
	{
		std::string buffer;

		buffer.assign(1, top_nibble((unsigned char)ch));
		buffer.append(1, bottom_nibble((unsigned char)ch));

		return buffer;
	}


	void nibblize(const char* source, std::string& destination, int length)
	{
		for ( int i = 0; i < length; ++i )
		{
			destination.append(1, top_nibble((unsigned char)source[i]));
			destination.append(1, bottom_nibble((unsigned char)source[i]));
		}
	}


	void unnibblize(const std::string& source, std::string& destination, std::string::size_type length)
	{
		if ( 2 * length > source.size() )
			length = source.size() / 2;

		destination.reserve(length);

		for ( std::string::size_type i = 0, j = 0; i < length; ++i, j+=2 )
			destination.append(1, (char)(chartohexint(source[j]) << 4 | chartohexint(source[j+1])));
	}


	/* With the next release, the following uncommented two functions should be replaced with the
	commented ones that follow:
	void unnibblize(const std::string& source, char* destination, std::string::size_type length)
	{
		if ( 2 * length > source.size() )
			length = source.size() / 2;

		for ( std::string::size_type i = 0, j = 0; i < length; ++i, j+=2 )
			destination[i] = (char)(chartohexint(source[j]) << 4 | chartohexint(source[j+1]));
	}


	void unnibblize(const char* source, char* destination, std::string::size_type length)
	{
		for ( std::string::size_type i = 0, j = 0; i < length; ++i, j+=2 )
			destination[i] = (char)(chartohexint(source[j]) << 4 | chartohexint(source[j+1]));
	}*/


	void unnibblize(const std::string& source, char* destination, int length)
	{
		int i, j;

		for ( j = i = 0; i < length; ++i, j+=2 )
			destination[i] = (char)(chartohexint(source[j]) * 16 + chartohexint(source[j+1]));
	}


	void unnibblize(const char* source, char* destination, int length)
	{
		int i, j;

		for ( j = i = 0; i < length; ++i, j+=2 )
			destination[i] = (char)(chartohexint(source[j]) * 16 + chartohexint(source[j+1]));
	}


	char hextochar(const char ch)
	{
		return (char)chartohexint(ch);
	}


	char hextochar(const char ch1, const char ch2)
	{
		return (char)(chartohexint(ch1) * 16 + chartohexint(ch2));
	}


#ifndef NO_NAMESPACES
} // namespace util_string
#endif

