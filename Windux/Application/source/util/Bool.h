/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef BOOL_H
#define BOOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define BOOL_STR_SIZE 2
#define BOOL_TRUE_STR   "1"
#define BOOL_FALSE_STR  "0"
#define BOOL_TRUE_STR1  "T"
#define BOOL_FALSE_STR1 "F"
#define BOOL_TRUE_STR2  "t"
#define BOOL_FALSE_STR2 "f"
#define BOOL_TRUE_STR3  "y"
#define BOOL_FALSE_STR3 "n"
#define BOOL_TRUE_STR4  "Y"
#define BOOL_FALSE_STR4 "N"

class Bool
{
	public:
		Bool() {init(false);}
		Bool(bool i) {init(i);}
		void init(bool i) {value=i;}
		static bool valueOf(const char *i) {return (i == (char *)0) ? false :
															(((*i == *BOOL_TRUE_STR)  ||
															  (*i == *BOOL_TRUE_STR1) ||
															  (*i == *BOOL_TRUE_STR2) ||
															  (*i == *BOOL_TRUE_STR3) ||
															  (*i == *BOOL_TRUE_STR4)) ? true : false);}
		static bool valueOf(const std::string& s) {return (s.empty()) ? false :
																	(((s[0] == *BOOL_TRUE_STR)  ||
																	  (s[0] == *BOOL_TRUE_STR1) ||
																	  (s[0] == *BOOL_TRUE_STR2) ||
																	  (s[0] == *BOOL_TRUE_STR3) ||
																	  (s[0] == *BOOL_TRUE_STR4)) ? true : false);}

		static const char *toString(bool i){return((i) ? BOOL_TRUE_STR : BOOL_FALSE_STR);}
		const char *toString() {return((value) ? BOOL_TRUE_STR : BOOL_FALSE_STR);}

	private:
		bool value;
};

#endif //BOOL_H

