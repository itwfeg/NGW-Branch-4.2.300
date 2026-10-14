#include "collate_en.h"



#ifndef NO_NAMESPACES
namespace std
{
#endif



int collate_en::do_compare(const char* b, const char* e, const char* b2, const char* e2) const
{
	for ( ; b < e && b2 < e2; ++b, ++b2 )
	{
		if ( *b < *b2 )
			return -1;
		else if ( *b > *b2 )
			return 1;
	}
		
	return 0;
}


collate_en::string_type collate_en::do_transform(const char* b, const char* e) const
{
	return string_type(b, e);
}



#ifndef NO_NAMESPACES
}
#endif

