/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef INTEGER_H
#define INTEGER_H

#ifdef PAL_WIN32_RUN
#pragma warning (disable:4996)
#endif

#include "../db/util/e_string/utilstring.h"

#define INT_STR_SIZE 13

class Integer
{
    public:
        Integer() {init(0);};
        Integer(int i) {init(i);};
        void init(int i) {value=i; memset(str,0,sizeof(str));};
        static int valueOf(const char *i) {return (i == (char *)0) ? 0 : atoi(i);};
        static int valueOf(const std::string& s) {return atoi(s.c_str());};
        char *toString() {sprintf(str,"%d",value); return str;};
        
#if INTPTR_MAX == INT64_MAX
		static std::string toString(long long i) {
			return util_string::longlongtostring(i);
		};
#else
		static std::string toString(int i){
			return util_string::inttostring(i);
		};
#endif           
		static std::string toString(int i, int width){
			return util_string::inttostring(i,width);
		};
		
		static std::string toString64(__int64 i){
			return util_string::int64tostring(i);
        };
    
	private:
            int value;
            char str[INT_STR_SIZE];
};

#endif //INTEGER_H

