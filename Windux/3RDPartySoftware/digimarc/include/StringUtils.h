#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include "dsmsdk.h"

class StringUtils {
	public:
		DSM_API_METHOD static bool MyStringOnlyContainsDigits(const char* Input);
};

#endif
