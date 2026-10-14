/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ULONG_H
#define ULONG_H

#include "../db/util/e_string/utilstring.h"

#define ULONG_STR_SIZE 13

class ULong
{
	public:
		ULong() {init(0L);};
		ULong(unsigned long l) {init(l);};
		void init(unsigned long l) {value=l; memset(str,0,sizeof(str));};
		static unsigned long valueOf(const char *l) {return (l == (char *)0) ? 0L : strtoul(l, (char **)NULL, 10);};
		static int valueOf(const std::string& s) {return ULong::valueOf(s.c_str());};
		char *toString() {sprintf(str,"%lu",value); return str;};
		
		static std::string toString(unsigned long l)
		{
			return util_string::ulongtostring(l);
		};

	private:
		unsigned long value;
		char str[ULONG_STR_SIZE];
};

#endif //ULONG_H

