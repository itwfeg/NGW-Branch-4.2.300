/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef LONG_H
#define LONG_H

#include "../db/util/e_string/utilstring.h"

#define LONG_STR_SIZE 13

class Long
{
	public:
		Long() {init(0L);};
		Long(long l) {init(l);};
		void init(long l) {value=l; memset(str,0,sizeof(str));};
		static long valueOf(const char *l) {return (l == (char *)0) ? 0L : atol(l);};
		static int valueOf(const std::string& s) {return Long::valueOf(s.c_str());};
		char *toString() {sprintf(str,"%ld",value); return str;};

#if INTPTR_MAX == INT64_MAX
		static std::string toString(long long l)
		{
			return util_string::longlongtostring(l);
		};
#else
		static std::string toString(long l)
		{
			return util_string::longtostring(l);
		};
#endif

	private:
		long value;
		char str[LONG_STR_SIZE];
};

#endif //LONG_H

