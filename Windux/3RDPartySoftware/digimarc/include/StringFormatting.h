#ifndef STRING_FORMATTING_H
#define STRING_FORMATTING_H

#include "dsmsdk.h"

class StringFormatting {
	public:
		DSM_API_METHOD static DSM_RESULT MySnprintf(char* OutBuffer, int OutBuffLen, const char* Format, ...);
};

#endif
