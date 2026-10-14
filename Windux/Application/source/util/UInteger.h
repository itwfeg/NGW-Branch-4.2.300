/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef UINTEGER_H
#define UINTEGER_H

#include "../db/util/e_string/utilstring.h"
#define UINT_STR_SIZE 13

class UInteger
{
    public:
#if defined ( __linux__ )
        UInteger():value(0) {
            memset(str,0,sizeof(str));
        }
        UInteger(unsigned int i):value(i) {
            memset(str,0,sizeof(str));
        }
#else
        UInteger() {init(0);};
        UInteger(unsigned int i) {init(i);};
        void init(unsigned int i) {value=i; memset(str,0,sizeof(str));};
#endif
        static unsigned int valueOf(const char *i) {return (i == (char *)0) ? 0 : atoi(i);};
        char *toString() {sprintf(str,"%u",value); return str;};
        
		static std::string toString(unsigned int i)
		{
			return util_string::uinttostring(i);
		};

    private:
            unsigned int value;
            char str[UINT_STR_SIZE];
};

#endif //UINTEGER_H

