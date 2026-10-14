/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef DOUBLE_H
#define DOUBLE_H

#if defined(LINUX_FIX)
    #include "PAL_platformheader.h"
#endif

#include "platform/platform.h"
#include <locale>
#include "../db/util/e_string/utilstring.h"

#define DOUBLE_STR_SIZE 30

using namespace util_platform;
using namespace std;

class Double
{
	public:
		Double() :
			value((double)0),
			minWidth(DOUBLE_STR_SIZE),
			precision(DOUBLE_STR_SIZE),
			str((char *)NULL)
		{};
		Double(double d) :
			value(d),
			minWidth(DOUBLE_STR_SIZE),
			precision(DOUBLE_STR_SIZE),
			str((char *)NULL)
		{};
		~Double() {delete[] str;};

		static double valueOf(const char *d)
		{
			if (d == (char *)0)
				return 0;
			if (strchr(d, '.') == 0)
				return strtod(d, (char **)NULL);
			else
			{
				std::string localeStr("");
				Platform::getInstance().getLocaleString(localeStr, std::string("en"), std::string("US"));
				locale selectedLocale = locale(localeStr.c_str());
            #if defined(LINUX_FIX)
//				return PAL_absclass::PAL_StrTod_l(d, (char **)NULL, PAL_absclass::PAL_createLocale(LC_TIME, selectedLocale.name().c_str()));
				PAL_LOCALE_TYPE l_type;
				l_type = PAL_absclass::PAL_createLocale(LC_TIME, selectedLocale.name().c_str());
				double str_d=0;
				str_d = _strtod_l(d, (char **)NULL, l_type);
				PAL_absclass::PAL_freeLocale(l_type);
				return str_d;
            #else
				return _strtod_l(d, (char **)NULL, _create_locale(LC_TIME, selectedLocale.name().c_str()));
            #endif
			}
		};
		static double valueOf(const std::string& s) {return valueOf(s.c_str());};
				
		static std::string toString(double d, int minWidth, int precision)
		{
			return util_string::doubletostring(d, minWidth, precision);
		};

		double getValue(void) {return value;};
		void setValue(double d) {value=d;};
		int getMinWidth(void) {return minWidth;};
		void setMinWidth(int w) {if (str != (char *)NULL) delete str; minWidth=w;};
		int getPrecision(void) {return precision;};
		void setPrecision(int p) {precision=p;};
		char *toString()
			{if (str==(char *)NULL) str=new char[minWidth+10];
			 memset(str,0,minWidth+10);
			 sprintf(str,"%*.*f", minWidth, precision, value);
			 return str;};

	private:
		double value;
		int minWidth, precision;
		char *str;
};

#endif //DOUBLE_H
