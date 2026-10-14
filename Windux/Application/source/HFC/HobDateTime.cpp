/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PAL_platformheader.h"

#include "HobDateTime.h"
#include "HobTokenizerCSV.h"
#include "Configuration.h"
#include "platform/platform.h"
#include "HobString.h"
#include "Integer.h"
#include "Long.h"
#include "ULong.h"
#include "Hex.h"
#include "PythonScripter.h"
#include <algorithm>
#include <iterator>
#include <locale>
#include <iostream>
#include <string>
#include <vector>

#define HOBART_440MXPC_TARGET

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobDateTime::dataType = 0;
std::string HobDateTime::dataTypeName("HobDateTime"); 
#endif

short HobDateTime::initialized = 0;

#if defined ( __linux__ )
char *HobDateTime::twoCharMonthDescEN[13] =
{
	(char *)("JA"), (char *)("FE"), (char *)("MR"), (char *)("AL"), (char *)("MA"), (char *)("JN"), (char *)("JL"), (char *)("AU"), (char *)("SE"), (char *)("OC"), (char *)("NO"), (char *)("DE")
};

char *HobDateTime::twoCharMonthDescES[13] =
{
	(char *)("EN"), (char *)("EN"), (char *)("MR"), (char *)("AL"), (char *)("MA"), (char *)("JN"), (char *)("JL"), (char *)("AU"), (char *)("SE"), (char *)("OC"), (char *)("NO"), (char *)("DI")
};

char *HobDateTime::twoCharMonthDescFR[13] =
{
	(char *)("JA"), (char *)("FE"), (char *)("MR"), (char *)("AL"), (char *)("MA"), (char *)("JN"), (char *)("JL"), (char *)("AU"), (char *)("SE"), (char *)("OC"), (char *)("NO"), (char *)("DÉ")
};

char *HobDateTime::threeCharMonthDescEN[13] =
{
	(char *)("Jan"), (char *)("Feb"), (char *)("Mar"), (char *)("Apr"), (char *)("May"), (char *)("Jun"), (char *)("Jul"), (char *)("Aug"), (char *)("Sep"), (char *)("Oct"), (char *)("Nov"), (char *)("Dec")
};
char *HobDateTime::threeCharMonthDescES[13] =
{
	(char *)("Ene"), (char *)("Feb"), (char *)("Mar"), (char *)("Abr"), (char *)("May"), (char *)("Jun"), (char *)("Jul"), (char *)("Ago"), (char *)("Sep"), (char *)("Oct"), (char *)("Nov"), (char *)("Dic")
};
char *HobDateTime::threeCharMonthDescFR[13] =
{
	(char *)("Jan"), (char *)("Fév"), (char *)("Mar"), (char *)("Avr"), (char *)("Mai"), (char *)("Jun"), (char *)("Jul"), (char *)("Aoû"), (char *)("Sep"), (char *)("Oct"), (char *)("Nov"), (char *)("Déc")
};
#else
char *HobDateTime::twoCharMonthDescEN[13] = {"JA", "FE", "MR", "AL", "MA", "JN", "JL", "AU", "SE", "OC", "NO", "DE"};
char *HobDateTime::twoCharMonthDescES[13] = {"EN", "EN", "MR", "AL", "MA", "JN", "JL", "AU", "SE", "OC", "NO", "DI"};
char *HobDateTime::twoCharMonthDescFR[13] = {"JA", "FE", "MR", "AL", "MA", "JN", "JL", "AU", "SE", "OC", "NO", "DÉ"};

char *HobDateTime::threeCharMonthDescEN[13] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
char *HobDateTime::threeCharMonthDescES[13] = {"Ene", "Feb", "Mar", "Abr", "May", "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"};
char *HobDateTime::threeCharMonthDescFR[13] = {"Jan", "Fév", "Mar", "Avr", "Mai", "Jun", "Jul", "Aoû", "Sep", "Oct", "Nov", "Déc"};

#endif

using namespace util_platform;
using namespace std;

HobDateTime::HobDateTime(time_t nsec)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	if (nsec > 0)
	{
		setCalendarTime(nsec);
	}
}

#if defined(LINUX_FIX)
	HobDateTime::HobDateTime(const HobDateTime &hdt)
: HfcObject()
#else
	HobDateTime::HobDateTime(const HobDateTime &hdt)
#endif
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	currTime = hdt.currTime;

	//	currTimeParts = hdt.currTimeParts;
	memcpy((struct tm *)&currTimeParts, (struct tm *) &hdt.currTimeParts, sizeof(currTimeParts));
}

HobDateTime::HobDateTime(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case BrowserFormat:
			{
				// format : 4DigitYear, month, day, hour, minute, hourIndicator

				HobString temp(data, len);
				HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
				HobString token;
				short loopCnt = 1;
				while (tok.next(&token) != -1)
				{
					setReceived(loopCnt, (token.length() == 0) ? false : true);

					switch (loopCnt)
					{
						case fieldOffsetYear:
							setYear(Integer::valueOf(token));
							break;
						case fieldOffsetMonth:
							setMonth(Integer::valueOf(token));
							break;
						case fieldOffsetDay:
							setDay(Integer::valueOf(token));
							break;
						case fieldOffsetHour:
							setHour(Integer::valueOf(token));
							break;
						case fieldOffsetMinute:
							setMinute(Integer::valueOf(token));
							break;
						case fieldOffsetHourIndicator:
							{
								/* if nothing received, or just blanks received, then the length of the token
								 * should be zero, since the CSV tokenizer removes leading and trailing blanks
								 * by default.
								 */
								HobString hourInd = token;
								if (hourInd == AMINDICATOR || hourInd == AMINDICATOR_CAPS)
									setHourIndicator(0);
								else if (hourInd == PMINDICATOR || hourInd == PMINDICATOR_CAPS)
									setHourIndicator(1);
								else
									setHourIndicator(2);
							}
							break;
						default:
							break;
					}
					loopCnt++;
				}
				break;
			}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

std::string HobDateTime::convertToTransactionDateFormat() const
{
	std::string dbFormattedStr("");
		
		//	We save the date in this format MM/DD/YYYY, but to request filtered transactions,
		//	the date format needs to be YYYY/MM/DD. If you pass in the date from the database,
		//	you get an exception. I don't understand why the format has to be different to read
		//	filtered transactions. For now, this is my workaround. [CCD April 2010]
		char strDate[12] = {'\0'};
		sprintf(strDate, "%04d/%02d/%02d", getYear4(), getMonth(), getDay());
		dbFormattedStr.append(HobString(strDate));
		dbFormattedStr.append(",");
		dbFormattedStr.append(formatTime(FormatTime_HHMMSS, HourFormat_MILITARY));
		
		return dbFormattedStr;
}

HobDateTime::HobDateTime(long longGregDate, const short formatIndicator)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	valid = true;
	int year, month, day;

	switch(formatIndicator)
	{
		//		case CANADIAN_DATE_YY_MO_DD:	//	This format is not set up in HobDateTime. Why?
		case FormatDate_YYMMDD:
			{
				year = (int)(longGregDate / 10000);
				longGregDate -= year * 10000;
				month = (int)(longGregDate / 100);
				day = (int)(longGregDate - (month * 100));
			}
			break;

		case FormatDate_DDmmmYY:
		case FormatDate_DDMMYY:
			{
				day = (int)(longGregDate / 10000);
				longGregDate -= day * 10000;
				month = (int)(longGregDate / 100);
				year = (int)(longGregDate - (month * 100));
			}
			break;

		case FormatDate_YYYYMMDD_dashes:
		case FormatDate_YYYYMMDD:
		case FormatDate_mmDDYY:
		case FormatDate_mmmDDYY:
		case FormatDate_MMDDYY:
		default:
			{
				month = (int)(longGregDate / 10000);
				longGregDate -= month * 10000;
				day = (int)(longGregDate / 100);
				year = (int)(longGregDate - (day * 100));
			}
			break;
	}
	if (year < 100)			//	Not sure why Ultima based value on 1997.
		year += 100;
	setYear(year);

	if(month > 12)
		month = 1;
	setMonth(month);

	if(day > 31)
		day = 1;
	setDay(day);
}


HobDateTime HobDateTime::createDateTime(const std::string& date, DateFormats dateFormat, TimeFormats timeFormat)
{
	HobDateTime dt;

	dt.convertFrom(date, dateFormat, timeFormat);

	return dt;
}


HobDateTime HobDateTime::createDateTimeFromDBFormat(const std::string& date)
{
	HobDateTime dt;

	dt.convertFromDBFormat(date);

	return dt;
}


HobDateTime::~HobDateTime()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


void HobDateTime::init()
{

	time(&currTime);

	currTimeParts = *localtime( &currTime );

	valid = true;
}


/******************************************************************************
 *
 * Formatting Routines
 *
 *****************************************************************************/

HobString HobDateTime::formatDate(const short formatIndicator, int usingWhichLocale) const
{
	char strDate[20] = {'\0'};

	HobDateLocales mylocale;
	std::string localeStr("");

	if (usingWhichLocale == _UsingPrimaryLanguageLocale_)
		Platform::getInstance().getLocaleString(localeStr, Configuration::getInstance()->getPrimaryLanguageCode(),
			Configuration::getInstance()->getPrimaryCountryCode());
	else
		Platform::getInstance().getLocaleString(localeStr, Configuration::getInstance()->getSecondaryLanguageCode(),
			Configuration::getInstance()->getSecondaryCountryCode());

	// Implemented this like this because I see some future changes coming to 2 digit dates and Windows version
	if ( ("en_US.utf8" == localeStr)       || ("English_United States" == localeStr) )
		mylocale = fieldOffsetLocale_EN_US;
	else if ( ("es_MX.utf8" == localeStr)  || ("Spanish_Mexico" == localeStr) )
		mylocale = fieldOffsetLocale_ES_MX;
	else if ( ("fr_CA.utf8" == localeStr)  || ("French_Canada"  == localeStr) )
		mylocale = fieldOffsetLocale_FR_CA;
	else if ( ("en_CA.utf8" == localeStr)  || ("English_Canada" == localeStr) )
		mylocale = fieldOffsetLocale_EN_US;

	switch (formatIndicator)
	{
		// Browser must internationalize !!!
		case FormatDate_mmDDYY:  // 0 = DE 25, 92   -  FormatDate_mmDDYY
		{
			char monthDesc[3];
			strcpy(monthDesc, twoCharMonthDescEN[getMonth() - 1]);
			sprintf(strDate, "%s %02d, %02d ", monthDesc, getDay(), getYear2());
			break;
		}

		// Browser must internationalize !!!
		case FormatDate_mmmDDYY:  // 1 = Dec 25, 92  -  FormatDate_mmmDDYY
		{
			char monthDesc[10];

			if (usingWhichLocale == _UsingCurrentLocale_)
				strftime(monthDesc, sizeof(monthDesc), "%b", &currTimeParts);
			else
			{
				Debug::trace("HobDateTime.locale", HobString("LINUX BF newlocale() localeStr: ") + HobString(localeStr));
				if (      fieldOffsetLocale_EN_US == mylocale)
					strcpy(monthDesc, threeCharMonthDescEN[getMonth() - 1]);
				else if ( fieldOffsetLocale_ES_MX == mylocale)
					strcpy(monthDesc, threeCharMonthDescES[getMonth() - 1]);
				else if ( fieldOffsetLocale_FR_CA == mylocale)
					strcpy(monthDesc, threeCharMonthDescFR[getMonth() - 1]);
				Debug::trace("HobDateTime.locale", HobString("LINUX AF strftime_l() monthDesc: ") + HobString((char*)monthDesc));
			}

			sprintf(strDate, "%s %02d, %02d ", monthDesc, getDay(), getYear2());
			break;
		}

		case FormatDate_YYMMDD:  // 2 = 92/12/25	-  FormatDate_YYMMDD
			sprintf(strDate, "%02d/%02d/%02d ", getYear2(), getMonth(), getDay());
			break;

		case FormatDate_YYYYMMDD:  // 4 = 1992{12|25} -  FormatDate_YYYYMMDD
			sprintf(strDate, "%04d{%02d|%02d} ", getYear4(), getMonth(), getDay());
			break;

			// Browser must internationalize !!!
		case FormatDate_DDmmmYY:  // 5 = 25/Dec/92   -  FormatDate_DDmmmYY
		{
			char monthDesc[10];

			if (usingWhichLocale == _UsingCurrentLocale_)
				strftime(monthDesc, sizeof(monthDesc), "%b", &currTimeParts);
			else
			{
				Debug::trace("HobDateTime.locale", HobString("LINUX BF newlocale() localeStr: ") + HobString(localeStr));
				if (      fieldOffsetLocale_EN_US == mylocale)
					strcpy(monthDesc, threeCharMonthDescEN[getMonth() - 1]);
				else if ( fieldOffsetLocale_ES_MX == mylocale)
					strcpy(monthDesc, threeCharMonthDescES[getMonth() - 1]);
				else if ( fieldOffsetLocale_FR_CA == mylocale)
					strcpy(monthDesc, threeCharMonthDescFR[getMonth() - 1]);
				Debug::trace("HobDateTime.locale", HobString("LINUX AF strftime_l() monthDesc: ") + HobString((char*)monthDesc));
			}

			sprintf(strDate, "%02d/%s/%02d ", getDay(), monthDesc , getYear2());
			break;
		}

		case FormatDate_DDMMYY:  // 6 = 25/12/92	-  FormatDate_DDMMYY
			sprintf(strDate, "%02d/%02d/%02d ", getDay(), getMonth(), getYear2());
			break;

			// Browser must internationalize !!!
		case FormatDate_YYmmDD:  // 0 = 92 DE 25   -  FormatDate_YYmmDD : CANADIAN_DATE_YY_MO_DD
		{
			/*
			   if (use_CENTURY)
			   {
			   memcpy(Txt,yyyy,4);
			   Txt += 4;
			   }
			   else
			   {
			   memcpy(Txt,&yyyy[2],2);
			   Txt += 2;
			   }
			 *Txt++ = ' ';

			 memcpy(Txt,CANMonth+((Month - 1) * 2),2);
			 Txt += 2;
			 *Txt++ = ' ';

			 memcpy(Txt,dd,2);
			 */

			char monthDesc[3];
			strcpy(monthDesc, twoCharMonthDescEN[getMonth() - 1]);
			sprintf(strDate, "%02d %s %02d ", getYear2(), monthDesc, getDay());
			break;
		}
		case FormatDate_YYMMDD_NoDelimiter:
			sprintf(strDate, "%02d%02d%02d", getYear2(), getMonth(), getDay());
			break;
		case FormatDate_MMDDYY_NoDelimiter:  // 97 = 122502	-  FormatDate_MMDDYY_NoDelimiter
			sprintf(strDate, "%02d%02d%02d", getMonth(), getDay(), getYear2());
			break;

		case FormatDate_MMDDYYYY:
			sprintf(strDate, "%02d/%02d/%04d ", getMonth(), getDay(), getYear4());
			break;

		case FormatDate_DDD:
			sprintf(strDate, "%3d ", getDayOfYear());
			break;

		case FormatDate_DD_mmm_YY:  // 8 = 25 DEC 92
		{
			char monthDesc[10];

			if (usingWhichLocale == _UsingCurrentLocale_)
				strftime(monthDesc, sizeof(monthDesc), "%b", &currTimeParts);
			else
			{
				Debug::trace("HobDateTime.locale", HobString("LINUX BF newlocale() localeStr: ") + HobString(localeStr));

				if (      fieldOffsetLocale_EN_US == mylocale)
					strcpy(monthDesc, threeCharMonthDescEN[getMonth() - 1]);
				else if ( fieldOffsetLocale_ES_MX == mylocale)
					strcpy(monthDesc, threeCharMonthDescES[getMonth() - 1]);
				else if ( fieldOffsetLocale_FR_CA == mylocale)
					strcpy(monthDesc, threeCharMonthDescFR[getMonth() - 1]);

				Debug::trace("HobDateTime.locale", HobString("LINUX AF strftime_l() monthDesc: ") + HobString((char*)monthDesc));
			}

			HobString upperMonthDesc(monthDesc);
			upperMonthDesc.toUpper();
			sprintf(strDate, "%02d %s %02d ", getDay(), upperMonthDesc.c_str(), getYear2());
			break;
		}

		case FormatDate_YYYYMMDD_dashes:
			sprintf(strDate, "%04d-%02d-%02d ", getYear4(), getMonth(), getDay());
			break;

		case FormatDate_MMDDYY:  // 3 = 12/25/02	-  FormatDate_MMDDYY
		default:
			sprintf(strDate, "%02d/%02d/%02d ", getMonth(), getDay(), getYear2());
			break;
	}
	return HobString(strDate);
}

HobString HobDateTime::formatTime(const short formatIndicator) const
{
	return formatTime(formatIndicator, Configuration::getInstance()->getTimeDisplayFormat());
}

HobString HobDateTime::formatTime(const short formatIndicator, const short hoursIndicator) const
{
	char strTime[15] = {'\0'};
	switch (formatIndicator)
	{
		case FormatTime_HHMMSS:
			if (hoursIndicator == HourFormat_AMPM)
			{
				sprintf(strTime, "%02d:%02d:%02d ", getHour12(), getMinute(), getSecond());
				strcat(strTime, ((getHour24() < 12) ? AMINDICATOR_CAPS : PMINDICATOR_CAPS));
			}
			else /*HourFormat_MILITARY*/
			{
				sprintf(strTime, "%02d:%02d:%02d", getHour24(), getMinute(), getSecond());
			}
			break;

		case FormatTime_HHMMSS_NoDelimiter:
			if (hoursIndicator == HourFormat_AMPM)
			{
				sprintf(strTime, "%02d%02d%02d", getHour12(), getMinute(), getSecond());
				strcat(strTime, ((getHour24() < 12) ? AMINDICATOR_CAPS : PMINDICATOR_CAPS));
			}
			else /*HourFormat_MILITARY*/
			{
				sprintf(strTime, "%02d%02d%02d", getHour24(), getMinute(), getSecond());
			}
			break;

		case FormatTime_HHMM:
		default:
			if (hoursIndicator == HourFormat_AMPM)
			{
				sprintf(strTime, "%02d:%02d ", getHour12(), getMinute());
				strcat(strTime, ((getHour24() < 12) ? AMINDICATOR_CAPS : PMINDICATOR_CAPS));
			}
			else /*HourFormat_MILITARY*/
			{
				sprintf(strTime, "%02d:%02d", getHour24(), getMinute());
			}
			break;
	}
	return HobString(strTime);
}


HobString HobDateTime::formatDateTime(const short formatIndicator, int usingWhichLocale) const
{
	HobString temp = formatDate(formatIndicator, usingWhichLocale);
	temp.append(formatTime(formatIndicator));

	return HobString(temp);
}


HobString HobDateTime::formatDateTime(const short dateFormatIndicator, const short timeFormatIndicator, int usingWhichLocale) const
{
	HobString temp = formatDate(dateFormatIndicator, usingWhichLocale);
	temp.append(formatTime(timeFormatIndicator));

	return HobString(temp);
}


/******************************************************************************
 *
 * End of Formatting Routines
 *
 *****************************************************************************/


// According to time.h, tm_year is number of years since 1900.
int HobDateTime::getYear() const
{
	return currTimeParts.tm_year;
}
int HobDateTime::getYear2() const
{
	return (currTimeParts.tm_year > 99) ? currTimeParts.tm_year - 100 : currTimeParts.tm_year;
}
int HobDateTime::getYear4() const
{
	return currTimeParts.tm_year + 1900;
}
void HobDateTime::setYear(int year)
{
	Debug::trace("HobDateTime", HobString("setYear()  ") + HobString(Integer::toString(year)));
	if (year > 1900)
		year -= 1900;
	currTimeParts.tm_year = year;
}

int HobDateTime::getMonth() const
{
	return currTimeParts.tm_mon + 1;
}
void HobDateTime::setMonth(int month)
{
	// I'm expecting the month to be between 1 (Jan.) and 12 (Dec.),
	// but the structure expects it to be 0-based, thus, subtract 1 from the month.
	month -= 1;
	if (month < 0 || month > 11)
	{
		valid = false;
		month = 0; // This would be an error condition, but check for valid data anyway.
	}

	Debug::trace("HobDateTime", HobString("setMonth()  ") + HobString(Integer::toString(month)));
	currTimeParts.tm_mon = month;
}

// Get the day-of-the-week.  Returns the number of days since Sunday;
// i.e. Sunday = 0, ... Saturday = 6.
int HobDateTime::getDayOfWeek() const
{
	return currTimeParts.tm_wday;
}

std::string HobDateTime::getDayOfWeekString(int usingWhichLocale)
{
	char dowDesc[30];

	if (usingWhichLocale == _UsingCurrentLocale_)
		strftime(dowDesc, 29, "%A", &currTimeParts);
	else
	{
		std::string localeStr("");
		if (usingWhichLocale == _UsingPrimaryLanguageLocale_)
			Platform::getInstance().getLocaleString(localeStr, Configuration::getInstance()->getPrimaryLanguageCode(),
					Configuration::getInstance()->getPrimaryCountryCode());
		else
			Platform::getInstance().getLocaleString(localeStr, Configuration::getInstance()->getSecondaryLanguageCode(),
					Configuration::getInstance()->getSecondaryCountryCode());
#if defined ( __linux__ )
		locale_t myLocale = newlocale(LC_TIME, localeStr.c_str(), NULL);
		strftime_l( dowDesc, sizeof(dowDesc), "%A", &currTimeParts, myLocale);
#else
		locale selectedLocale = locale(localeStr.c_str());

		_strftime_l(dowDesc, 29, "%A", &currTimeParts, PAL_absclass::PAL_createLocale(LC_TIME, selectedLocale.name().c_str()));
#endif	  // __linux__
	}

	return std::string(dowDesc);
}

std::string HobDateTime::getDayOfWeekShortString(int usingWhichLocale)
{
	char dowDesc[30];

	if (usingWhichLocale == _UsingCurrentLocale_)
		strftime(dowDesc, 29, "%a", &currTimeParts);
	else
	{
		std::string localeStr("");
		if (usingWhichLocale == _UsingPrimaryLanguageLocale_)
			Platform::getInstance().getLocaleString(localeStr, Configuration::getInstance()->getPrimaryLanguageCode(),
					Configuration::getInstance()->getPrimaryCountryCode());
		else
			Platform::getInstance().getLocaleString(localeStr, Configuration::getInstance()->getSecondaryLanguageCode(),
					Configuration::getInstance()->getSecondaryCountryCode());
#if defined ( __linux__ )
		locale_t myLocale = newlocale(LC_TIME, localeStr.c_str(), NULL);
		strftime_l( dowDesc, sizeof(dowDesc), "%a", &currTimeParts, myLocale);
#else
		locale selectedLocale = locale(localeStr.c_str());

		_strftime_l(dowDesc, 29, "%a", &currTimeParts, PAL_absclass::PAL_createLocale(LC_TIME, selectedLocale.name().c_str()));
#endif  // __linux__
	}

	return std::string(dowDesc);
}


//short HobDateTime::getDay() const
//#else
//		_strftime_l(dowDesc, 29, "%a", &currTimeParts, _create_locale(LC_TIME, selectedLocale.name().c_str()));
//	}

//	return std::string(dowDesc);
//}


int HobDateTime::getDay() const
//#endif
{
	return currTimeParts.tm_mday;
}
void HobDateTime::setDay(int day)
{
	// This data is not zero based (like the month is).
	if (day < 1 || day > 31)
	{
		valid = false;
		day = 1;
	}

	Debug::trace("HobDateTime", HobString("setDay()  ") + HobString(Integer::toString(day)));
	currTimeParts.tm_mday = day;
}

// Same as getHour24; returns the hour in 24-hour format (0 through 23 inclusive).
int HobDateTime::getHour() const
{
	return currTimeParts.tm_hour;
}
// Returns the hour in 12-hour format (1 through 12 inclusive).
int HobDateTime::getHour12() const
{
	if (currTimeParts.tm_hour == 0)
		return 12;
	else if (currTimeParts.tm_hour <= 12)
		return currTimeParts.tm_hour;
	else
		return currTimeParts.tm_hour - 12;
}
// Returns the hour in 24-hour format (0 through 23 inclusive).
int HobDateTime::getHour24() const
{
	return getHour();
}
void HobDateTime::setHour(int hour)
{
	Debug::trace("HobDateTime", HobString("setHour()  ") + HobString(Integer::toString(hour)));
	if (hour >= 0 && hour <= 23)
		currTimeParts.tm_hour = hour;
}

int HobDateTime::getMinute() const
{
	return currTimeParts.tm_min;
}
void HobDateTime::setMinute(int minute)
{
	Debug::trace("HobDateTime", HobString("setMinute()  ") + HobString(Integer::toString(minute)));
	if (minute >= 0 && minute < 60)
		currTimeParts.tm_min = minute;
}

int HobDateTime::getSecond() const
{
	return currTimeParts.tm_sec;
}
void HobDateTime::setSecond(int second)
{
	Debug::trace("HobDateTime", HobString("setSecond()  ") + HobString(Integer::toString(second)));
	if (second >= 0 && second < 60)
		currTimeParts.tm_sec = second;
}

int HobDateTime::getHourIndicator() const
{
	return Configuration::getInstance()->getTimeDisplayFormat();
}
void HobDateTime::setHourIndicator(int hourIndicator)
{
	/* I received the hour indicator.  If I also received the hour, then update it based
	 * on the hour indicator (add 12 if PM, or subtract 12 for 12AM).
	 * Checking if I received the hour will only work if I have already received and
	 * processed the hour, which does happen to be the case.
	 */
	HobString hsch(Integer::toString(currTimeParts.tm_hour));
	Debug::trace("HobDateTime", HobString("setHourIndicator()  ") + HobString(Integer::toString(hourIndicator)) +
			HobString(" current hour is:") + hsch);

	if (hourIndicator == 0 && currTimeParts.tm_hour == 12)
		currTimeParts.tm_hour -= 12;
	else if (hourIndicator == 1 && currTimeParts.tm_hour >= 1 && currTimeParts.tm_hour <= 11)
		currTimeParts.tm_hour += 12;

	Debug::trace("HobDateTime", HobString("setHourIndicator()  new hour is:") + HobString(Integer::toString(currTimeParts.tm_hour)));
}


void HobDateTime::setToCurrent()
{
	Debug::trace("HobDateTime.vxworks", "setToCurrent()");

	time(&currTime);

	currTimeParts = *localtime( &currTime ); 
	if (Debug::isTracing("HobDateTime.new"))
	{
		time_t ct;
		time(&ct);
		struct tm  ctPart = *localtime( &ct );

		char strTime[50];
		strftime(strTime, 100, "%Y,%m,%d,%H,%M,%j", &currTimeParts);
		Debug::trace("HobDateTime.new", HobString("setToCurrent() - localtime is: ") + HobString(strTime));
	}
}

void HobDateTime::setToDate(std::string & /* newDate */)
{
	// Note: the format of the data in the string is dependant on the Configuration date-format setting.
}

void HobDateTime::incrementDays(int days)
{
	bool traceOn = Debug::isTracing("HobDateTime");
	HobString myDebug;
	if (traceOn)
		myDebug = "incrementDays():  ";

	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("BEFORE increment [").append(formatDate(FormatDate_MMDDYY)).append("]");
		Debug::trace("HobDateTime", myDebug2);
	}

	//	Too many problems rolling the date over to a new year with the vxworks time code.
	//	Instead, we will convert the current day to a julian day and add to that (CCD 2/8/05).
	long julDay = getJulianDay();
	setDateFromJulianDay(julDay + days);
	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("AFTER increment [").append(formatDate(FormatDate_MMDDYY)).append("]");
		Debug::trace("HobDateTime", myDebug2);
	}
}

void HobDateTime::incrementHours(int hours)
{
	currTimeParts.tm_hour += hours;
	fixDateRollover();

	if (Debug::isTracing("HobDateTime"))
	{
		Debug::trace("HobDateTime", HobString("incrementHours(): AFTER increment and fixDateRollover()"));
		show();
	}
}

void HobDateTime::incrementMinutes(int minutes)
{
	currTimeParts.tm_min += minutes;
	fixDateRollover();

	if (Debug::isTracing("HobDateTime"))
	{
		Debug::trace("HobDateTime", HobString("incrementMinutes(): AFTER increment and fixDateRollover()"));
		show();
	}
}

void HobDateTime::incrementSeconds(int seconds)
{
	currTimeParts.tm_sec += seconds;
	fixDateRollover();

	if (Debug::isTracing("HobDateTime"))
	{
		Debug::trace("HobDateTime", HobString("incrementSeconds(): AFTER increment and fixDateRollover()"));
		show();
	}
}

void HobDateTime::setTime()
{
	PAL_TIME_STRUCT st;
	st.wYear	= currTimeParts.tm_year + 1900;
	st.wMonth   = currTimeParts.tm_mon + 1;
	st.wDay	 = currTimeParts.tm_yday;
	st.wHour	= currTimeParts.tm_hour;
	st.wMinute  = currTimeParts.tm_min;
	st.wSecond  = currTimeParts.tm_sec;
	st.wDayOfWeek	   = currTimeParts.tm_wday;
	st.wMilliseconds	= 0;
	PAL_absclass::PAL_setLocalTime( &st );
}


const char *HobDateTime::serialize(int format) const
{
	Debug::trace("HobDateTime", HobString("serialize - sec:") + HobString(ULong::toString((unsigned long)currTime)));

	switch (format)
	{
		case BrowserFormat:
			{
				Debug::trace("HobDateTime", "serializing for browser format");
				serializedData = "";

				char strTime[50];
				if (Configuration::getInstance()->getTimeDisplayFormat() == HourFormat_AMPM)
				{
					// The following did not convert the %P correctly:   strftime(strTime, 100, "%Y,%m,%d,%I,%M,%P", &currTimeParts);
					strftime(strTime, 100, "%Y,%m,%d,%I,%M,", &currTimeParts);
					strcat(strTime, ((currTimeParts.tm_hour < 12) ? "AM" : "PM"));
				}
				else
					strftime(strTime, 100, "%Y,%m,%d,%H,%M, ", &currTimeParts);

				serializedData.append(HobString(strTime));
				Debug::trace("HobDateTime", HobString("done serializing for browser format:") + HobString(strTime));
			}
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


void HobDateTime::show(struct tm *timeParts) const
{
	if (Debug::isTracing("HobDateTime.show"))
	{
		if (timeParts)
		{
			HobString yr(Integer::toString(timeParts->tm_year));
			HobString mn(Integer::toString(timeParts->tm_mon));
			HobString dy(Integer::toString(timeParts->tm_mday));
			HobString hr(Integer::toString(timeParts->tm_hour));
			HobString mi(Integer::toString(timeParts->tm_min));
			HobString se(Integer::toString(timeParts->tm_sec));
			HobString wd(Integer::toString(timeParts->tm_wday));
			HobString yd(Integer::toString(timeParts->tm_yday));
			Debug::trace("HobDateTime.show", HobString("PASSED-IN: ") +
					HobString(" y,m,d,h,m,s,dow,doy: ") + yr +
					HobString(", ") + mn +
					HobString(", ") + dy +
					HobString(", ") + hr +
					HobString(", ") + mi +
					HobString(", ") + se +
					HobString(", ") + wd +
					HobString(", ") + yd);
		}
		else
		{
			HobString yr(Integer::toString(currTimeParts.tm_year));
			HobString mn(Integer::toString(currTimeParts.tm_mon));
			HobString dy(Integer::toString(currTimeParts.tm_mday));
			HobString hr(Integer::toString(currTimeParts.tm_hour));
			HobString mi(Integer::toString(currTimeParts.tm_min));
			HobString se(Integer::toString(currTimeParts.tm_sec));
			HobString wd(Integer::toString(currTimeParts.tm_wday));
			HobString yd(Integer::toString(currTimeParts.tm_yday));
			Debug::trace("HobDateTime.show", HobString("CURRENT: ") +
					HobString(" y,m,d,h,m,s,dow,doy: ") + yr +
					HobString(", ") + mn +
					HobString(", ") + dy +
					HobString(", ") + hr +
					HobString(", ") + mi +
					HobString(", ") + se +
					HobString(", ") + wd +
					HobString(", ") + yd);
		}
	}
}


void HobDateTime::operator=(const HobDateTime& hdt)
{
	currTime = hdt.currTime;

	//	currTimeParts = hdt.currTimeParts;
	memcpy((struct tm *)&currTimeParts, (struct tm *) &hdt.currTimeParts, sizeof(currTimeParts));
	valid = hdt.valid;
}

bool HobDateTime::operator==(const HobDateTime& hdt) const
{
	return (getMinute() == hdt.getMinute() &&
			getHour() == hdt.getHour() &&
			getDay() == hdt.getDay() &&
			getMonth() == hdt.getMonth() &&
			getYear() == hdt.getYear()) ? true : false;
}

bool HobDateTime::operator!=(const HobDateTime& hdt) const
{
	return !(*this == hdt);
}


bool HobDateTime::operator <  (const HobDateTime& rhs) const
{
	return getCalendarTime() < rhs.getCalendarTime();
}


bool HobDateTime::operator <= (const HobDateTime& rhs) const
{
	return getCalendarTime() <= rhs.getCalendarTime();
}


time_t HobDateTime::getCalendarTime() const
{
	return mktime((tm *) &currTimeParts);
}

void HobDateTime::setCalendarTime(time_t time)
{
	// This only sets this object's currTime and currTimeParts.
	// It does not write this to the real-time clock.
	currTime = time;
	//localtime_r(&currTime, &currTimeParts);
	currTimeParts =  *localtime( &currTime );
}


/*
   timeDiff() returns the absolute value of the difference between the 2 times.
   */
time_t HobDateTime::timeDiff(HobDateTime & time2) const
{
	long longTimeDiff = (long) (getCalendarTime() - time2.getCalendarTime());
	longTimeDiff = labs(longTimeDiff);
	return (time_t) longTimeDiff;
}

void HobDateTime::setupCurrTimeParts(time_t time)
{
	this->currTime = time;
	currTimeParts = *localtime( &currTime );
}


void HobDateTime::setDateParts(const HobString &newDate)
{
	if (newDate.find_first_of('/') != std::string::npos)
		setDateParts(newDate, FormatDate_MMDDYY);		// 12/25/99
	else
		setDateParts(newDate, FormatDate_mmmDDYY);	// Dec 25, 99
}

void HobDateTime::setDateParts(const HobString &newDate, const short format)
{
	HobString localNewDate = newDate;
	switch(format)
	{
		case FormatDate_YYYYMMDD:
		case FormatDate_YYMMDD:
			{
				HobTokenizerCSV tok(&localNewDate, "/-");
				HobString token;
				short loopCnt = 1;
				while (tok.next(&token) != -1)
				{
					switch (loopCnt)
					{
						case 1:		//	2 or 4 Digit year
							{
								int year = Integer::valueOf(token);
								if (year < 100) // 2 digit year
									setYear(year < 97 ? (year + 100) : year);
								else // 4 digit year
									setYear(year);
								break;
							}
						case 2:		//	2 Digit month
							setMonth(Integer::valueOf(token));
							break;
						case 3:		//	2 Digit day
							setDay(Integer::valueOf(token));
							break;
						default:
							break;
					}
					loopCnt++;
				}

				break;
			}
/*
		case FormatDate_YYYYMMDD_dashes:
			{
				HobTokenizerCSV tok(&localNewDate, '-');
				HobString token;
				short loopCnt = 1;
				while (tok.next(&token) != -1)
				{
					switch (loopCnt)
					{
						case 1:		//	2 or 4 Digit year
							{
								int year = Integer::valueOf(token);
								if (year < 100) // 2 digit year
									setYear(year < 97 ? (year + 100) : year);
								else // 4 digit year
									setYear(year);
								break;
							}
						case 2:		//	2 Digit month
							setMonth(Integer::valueOf(token));
							break;
						case 3:		//	2 Digit day
							setDay(Integer::valueOf(token));
							break;
						default:
							break;
					}
					loopCnt++;
				}

				break;
			}
*/
		case FormatDate_MMDDYYYY:
		case FormatDate_MMDDYY:
			{
				HobTokenizerCSV tok(&localNewDate, '/');
				HobString token;
				short loopCnt = 1;
				while (tok.next(&token) != -1)
				{
					switch (loopCnt)
					{
						case 1:		//	2 Digit month
							setMonth(Integer::valueOf(token));
							break;
						case 2:		//	2 Digit day
							setDay(Integer::valueOf(token));
							break;
						case 3:		//	2 or 4 Digit year
							{
								int year = Integer::valueOf(token);
								if (year < 100) // 2 digit year
									setYear(year < 97 ? (year + 100) : year);
								else // 4 digit year
									setYear(year);
								break;
							}
						default:
							break;
					}
					loopCnt++;
				}
				break;
			}

		case FormatDate_mmmDDYY:
		case FormatDate_mmDDYY:
			{
				localNewDate.stripWhiteSpace(HobString::LEADING_TRAILING);

				std::string::size_type spacePos = localNewDate.find_first_of(' ');
				if (spacePos == std::string::npos)
				{
					valid = false;
					break;
				}

				// month comes first, either 2 or 3 chars
				int myMonth = 0;
				HobString monthString = localNewDate.substr(0, spacePos);
				if (monthString.size() == 2)
				{
					HobString twoCharMonthDescString = "JAFEMRALMAJNJLAUSEOCNODE";
					std::string::size_type monthPos = twoCharMonthDescString.find_first_of(monthString);
					if (monthPos != std::string::npos)
					{
						myMonth = (int)(monthPos / 2);
						setMonth(myMonth+1);
					}
					else
					{
						valid = false;
						break;
					}
				}
				else
				{
					HobString threeCharMonthDescString = "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC";
					std::string::size_type monthPos = threeCharMonthDescString.find_first_of(monthString);
					if (monthPos != std::string::npos)
					{
						myMonth = (int)(monthPos / 3);
						setMonth(myMonth+1);
					}
					else
					{
						valid = false;
						break;
					}
				}

				std::string::size_type commaPos = localNewDate.find_first_of(',');
				int day = 1;
				if (commaPos != std::string::npos)
				{
					HobString dayString = localNewDate.substr(spacePos + 1, commaPos - spacePos - 1);
					day = Integer::valueOf(dayString);
					setDay(day);
				}
				else
				{
					valid = false;
					break;
				}

				if (commaPos < localNewDate.size())
				{
					HobString myYearString = localNewDate.substr(commaPos + 2);
					int myYear = Integer::valueOf(myYearString);
					if (myYear < 100) // 2 digit year
						setYear(myYear < 97 ? (myYear + 100) : myYear);
					else // 4 digit year
						setYear(myYear);
				}
				else
				{
					valid = false;
					break;
				}

				break;
			}

		case FormatDate_MMDDYY_NoDelimiter:  // 97 = 122502	-  FormatDate_MMDDYY_NoDelimiter
			{
				int month, day, year;

				sscanf(newDate.c_str(),"%02d%02d%02d", &month, &day, &year);

				setMonth(month);
				setDay(day);
				setYear(year);
			}

			break;

		default:
			valid = false;
			break;
	}
}

void HobDateTime::setDateParts(int month, int day, int year)
{
	setMonth(month);
	setDay(day);
	setYear(year);
	fixDateRollover();
}

void HobDateTime::setTimeParts(const HobString &newTime, const short formatIndicator, const short hoursIndicator)
{
	int hr = 0, min = 0, sec = 0;
	char am_pm[5];

	switch (formatIndicator)
	{
		case FormatTime_HHMMSS_NoDelimiter:

			if (hoursIndicator == HourFormat_AMPM)
			{
				sscanf (newTime.c_str(), "%d%d%d%s", &hr, &min, &sec, am_pm);
				std::string ap(am_pm);

				if ((ap.find(PMINDICATOR_CAPS) != std::string::npos) || (ap.find(PMINDICATOR) != std::string::npos))
					hr = (hr % 12) + 12;
				else
					hr = (hr % 12);

				setHour (hr);
				setMinute(min);
				setSecond(sec);

			}
			else /*HourFormat_MILITARY*/
			{
				sscanf (newTime.c_str(), "%d%d%d", &hr, &min, &sec);
				setHour(hr);
				setMinute(min);
				setSecond(sec);
			}
			break;

		case FormatTime_HHMMSS:
			if (hoursIndicator == HourFormat_AMPM)
			{
				sscanf (newTime.c_str(), "%d:%d:%d %s", &hr, &min, &sec, am_pm);
				std::string ap(am_pm);

				if ((ap.find(PMINDICATOR_CAPS) != std::string::npos) || (ap.find(PMINDICATOR) != std::string::npos))
					hr = (hr % 12) + 12;
				else
					hr = (hr % 12);

				setHour (hr);
				setMinute(min);
				setSecond(sec);

			}
			else /*HourFormat_MILITARY*/
			{
				sscanf (newTime.c_str(), "%d:%d:%d", &hr, &min, &sec);
				setHour(hr);
				setMinute(min);
				setSecond(sec);
			}
			break;


		case FormatTime_HHMM:
		default:
			if (hoursIndicator == HourFormat_AMPM)
			{
				sscanf (newTime.c_str(), "%d:%d %s", &hr, &min, am_pm);
				std::string ap(am_pm);

				if ((ap.find(PMINDICATOR_CAPS) != std::string::npos) || (ap.find(PMINDICATOR) != std::string::npos))
					hr = (hr % 12) + 12;
				else
					hr = (hr % 12);

				setHour (hr);
				setMinute(min);
				setSecond(0);

			}
			else /*HourFormat_MILITARY*/
			{
				sscanf (newTime.c_str(), "%d:%d", &hr, &min);
				setHour(hr);
				setMinute(min);
				setSecond(0);
			}
			break;
	}

}

void HobDateTime::setTimeParts(const HobString &newTime, const short format)
{
	switch(format)
	{
		case FormatTime_HHMM:
		case FormatTime_HHMMSS:
			{
				HobString temp = newTime;
				HobTokenizerCSV tok(&temp, ':');
				HobString token;
				short loopCnt = 1;
				while (tok.next(&token) != -1)
				{
					switch (loopCnt)
					{
						case 1:		//	2 Digit hour
							setHour(Integer::valueOf(token));
							break;
						case 2:		//	2 Digit minute
							setMinute(Integer::valueOf(token));
							break;
						case 3:		//	2 Digit seconds
							if (token.length() <= 2)
								setSecond(Integer::valueOf(token));
							else
							{
								setSecond(Integer::valueOf(token.substr(0,1)));

								//	Make adjustments for 12 hour time
								if ((token.find(AMINDICATOR) != std::string::npos) || (token.find(AMINDICATOR_CAPS) != std::string::npos))
									setHourIndicator(0);
								else if ((token.find(PMINDICATOR) != std::string::npos) || (token.find(PMINDICATOR_CAPS) != std::string::npos))
									setHourIndicator(1);
							}
							break;
						default:
							break;
					}
					loopCnt++;
				}
				fixDateRollover();
			}
			break;

		default:
			break;
	}
}

void HobDateTime::setTimeParts(int hour, int minute, int second)
{
	setHour(hour);
	setMinute(minute);
	setSecond(second);
	fixDateRollover();
}

void HobDateTime::convertFrom(const std::string& dateTime, DateFormats dateFormat, TimeFormats timeFormat)
{
	HobTokenizerCSV tok(&dateTime, BrowserFormatSeparator);
	HobString token;
	short loopCnt = 1;
	while (tok.next(&token) != -1)
	{
		switch (loopCnt)
		{
			case 1:		//	Date
				setDateParts(token, dateFormat);
				break;
			case 2:		//	Time
				setTimeParts(token, timeFormat);
				break;
			default:
				break;
		}
		loopCnt++;
	}
}


void HobDateTime::convertFromDBFormat(const HobString *dateTime)
{
	//	Current database date/time format is:	Sqlite 'YYYY-MM-DD HH:MM:SS'   // Codebase was MM/DD/YYYY,HH:MM:SS
	if (!dateTime)
		return;

	Debug::trace("HobDateTime.token", string("HobDateTime::convertFromDBFormat(HobString *):  '") + *dateTime + "'");

	if (dateTime->find(",") != std::string::npos)
	{
		// The date/time string is in the HLX format, so parse it as such.
		// This can happen when receiving (restore/merge/comm) data from an HLX or from an external Scale Management application.

		HobTokenizerCSV tok(dateTime, ",");
		HobString token;
		short loopCnt = 1;
		while (tok.next(&token) != -1)
		{
			switch (loopCnt)
			{
				case 1:		//	Date
					setDateParts(token, FormatDate_MMDDYYYY);
					break;
				case 2:		//	Time
					setTimeParts(token, FormatTime_HHMMSS);
					break;
				default:
					break;
			}
			loopCnt++;
		}
	}
	else
	{
		// The date/time string is in the HTi/HTx format, so parse it as such.
		// This really won't happen when receiving data via restore/merge, or from an external Scale Management application, since we write out (comm/backup) the date/time fields in the older HLX format,
		// but we could see it when receiving data from HT Scale Management, so we have support for it.

		HobTokenizerCSV tok(dateTime, " ");
		HobString token;
		short loopCnt = 1;
		while (tok.next(&token) != -1)
		{
			switch (loopCnt)
			{
				case 1:		//	Date
					setDateParts(token, FormatDate_YYYYMMDD);
					break;
				case 2:		//	Time
					setTimeParts(token, FormatTime_HHMMSS);
					break;
				default:
					break;
			}
			loopCnt++;
		}
	}
}


void HobDateTime::convertFromDBFormat(const std::string& dateTime)
{
	Debug::trace("HobDateTime.token", string("HobDateTime::convertFromDBFormat(string &):  '") + dateTime + "'");

	HobString temp(dateTime);
	convertFromDBFormat(&temp);
}


std::string HobDateTime::convertToDBFormat(DateTimeFormats format) const
{
	std::string dbFormattedStr("");

	if (format == FormatDateTime_Windux )
	{
		std::string::size_type position;

		//	formatDate places a space at the end of the Date string.
		//	We need to get rid of that space or else the time is invalid in the database.
		dbFormattedStr.append(formatDate(FormatDate_MMDDYYYY));
		position = dbFormattedStr.rfind(" ");
		while ((position != std::string::npos) && (position == (dbFormattedStr.length() - 1)))
		{
			dbFormattedStr.erase(position);
			position = dbFormattedStr.rfind(" ");
		}

		dbFormattedStr.append(",");
		dbFormattedStr.append(formatTime(FormatTime_HHMMSS, HourFormat_MILITARY));
	}
	else if (format == FormatDateTime_HTSeries)
	{
		dbFormattedStr.append(formatDate(FormatDate_YYYYMMDD_dashes));
		dbFormattedStr.append(formatTime(FormatTime_HHMMSS, HourFormat_MILITARY));
	}

	return dbFormattedStr;
}
/*
// For reference, this is the above method that Windux/HLX Legacy used.
// Sqlite didn't like this format for string representations of date/time,
// so we now use the above method for storing date/time strings into the database.
// We still need this method to format date/time strings for writing out to comm/backup.
std::string HobDateTime::convertToDBFormat() const
{
	std::string dbFormattedStr("");
	std::string::size_type position;

	//	formatDate places a space at the end of the Date string.
	//	We need to get rid of that space or else the time is invalid in the database.
	dbFormattedStr.append(formatDate(FormatDate_MMDDYYYY));
	position = dbFormattedStr.rfind(" ");
	while ((position != std::string::npos) && (position == (dbFormattedStr.length() - 1)))
	{
		dbFormattedStr.erase(position);
		position = dbFormattedStr.rfind(" ");
	}

	dbFormattedStr.append(",");
	dbFormattedStr.append(formatTime(FormatTime_HHMMSS, HourFormat_MILITARY));

	return dbFormattedStr;
}
*/


long HobDateTime::getLongGregDate(const short formatIndicator)
{
	long longGregDate;

	switch(formatIndicator)
	{
		//		case CANADIAN_DATE_YY_MO_DD:	//	This format is not set up in HobDateTime. Why?
		case FormatDate_YYMMDD:
			longGregDate = (getYear2() * 10000) + (getMonth() * 100) + getDay();
			break;

		case FormatDate_DDmmmYY:
		case FormatDate_DDMMYY:
			longGregDate = (getDay() * 10000) + (getMonth() * 100) + getYear2();
			break;

		case FormatDate_YYYYMMDD_dashes:
		case FormatDate_YYYYMMDD:
		case FormatDate_mmDDYY:
		case FormatDate_mmmDDYY:
		case FormatDate_MMDDYY:
		default:
			longGregDate = (getMonth() * 10000) + (getDay() * 100) + getYear2();
			break;
	}
	return longGregDate;
}

int HobDateTime::getDayOfYear() const
{
	return currTimeParts.tm_yday + 1;
}

long HobDateTime::getJulianDay() const
{
	long  century, xYear;
	int month = getMonth();
	int day = getDay();
	int year = getYear4();

	if (month <= 2)
	{
		--year;
		month += 12;
	}

	month -= 3;
	century = year / 100;
	xYear = year % 100;
	century = (century * JULIAN_D1) >> 2;
	xYear = (xYear * JULIAN_D0) >> 2;
	return(((((month * JULIAN_D4) + 2) / 5) + day) + JULIAN_D2 + xYear + century);
}

void HobDateTime::setDateFromJulianDay(long julianDay)
{
#if 1
	//	For the Gregorian calendar:
	long a = julianDay + JULIAN_D5;
	long b = (4 * a + 3) / JULIAN_D1;
	long c = a - (b * JULIAN_D1) / 4;
#else
	//	For the Julian calendar:
	long b = 0;
	long c = julianDay + JULIAN_D3;
#endif

	//	Then, for both calendars:
	long d = (4 * c + 3) / JULIAN_D0;
	long e = c - (JULIAN_D0 * d) / 4;
	long m = (5 * e + 2) / JULIAN_D4;

	int jDay = e - (JULIAN_D4 * m + 2) / 5 + 1;
	int jMonth = m + 3 - (12 * (m / 10));
	int jYear = b * 100 + d - 4800 + m / 10;

	if (Debug::isTracing("HobDateTime"))
	{
		HobString myDebug("setDateFromJulianDay(");
		HobString myMonth(Integer::toString(jMonth));
		HobString myDay(Long::toString(jDay));
		HobString myYear(Integer::toString(jYear));
		myDebug.append(Long::toString(julianDay)).append("):   Month = ").append(myMonth).append("   Day = ").append(myDay).append("   Year = ").append(myYear).append("\n\n");
		Debug::trace("HobDateTime", myDebug);
	}
	setMonth(jMonth);
	setDay(jDay);
	setYear(jYear);

	mktime(&currTimeParts);
}

bool HobDateTime::isLeapYear() const
{
	int year = getYear4();
	bool isLeapYear = false;

	//	Test for a leap year - A year completely divisable by 4 is typically a leap year.
	//						 - Exceptions to this: a year ending in "00" (i.e. "1700")
	//						   must be completely divisible by 400 to be a leap year.
	if ((year % 100) == 0)
	{
		if ((year % 400) == 0)
			isLeapYear = true;
	}
	else
		if ((year % 4) == 0)
		{
			isLeapYear = true;
		}
	return isLeapYear;
}

void HobDateTime::fixDateRollover()
{
	//	The following code is here to fix a bug in the vxWorks (v5.5) code for mktime.
	//	It was discovered that adding to the day (tm_mday) then calling mktime() would not calculate
	//	the currTimeParts properly if the resulting date crossed into the next year.
	//	Hopefully, this will fix the problem until Wind River fixes their stuff (CCD - 9/12/03).
	mktime(&currTimeParts);

	int daysInYear = isLeapYear() ? 366 : 365;

	while (getDayOfYear() > daysInYear)	//	Days since January 1
	{
		int year = getYear4();
		int day = getDay();

		year += 1;
		setYear(year);

		setMonth(0);

		day = getDayOfYear() - daysInYear;
		setDay(day);

		mktime(&currTimeParts);

		//	Set up to check the dayOfYear again.
		daysInYear = isLeapYear() ? 366 : 365;
	}
}


bool HobDateTime::isValid() const
{
	return valid;
}


// The following method returns the number of days from the current date/time until the date/time of this
// HobDateTime object.  Time of now and time of this object are irrelevant in determining the number of days.
// For any time of today, this method will return 0.  For any time of tomorrow, this method will return 1.
// For any time in the past, this method will return -1.
int HobDateTime::daysUntil(const HobDateTime &futureDateTime) const
{
	if (*this >= futureDateTime)
		return -1;

	HobDateTime copyOfFuture(futureDateTime);
	copyOfFuture.setHour(23);
	copyOfFuture.setMinute(59);
	copyOfFuture.setSecond(59);
	time_t diff = timeDiff(copyOfFuture);  // in seconds
#ifndef EVOL_NDEBUG
	Debug::trace("HobDateTime", HobString("difference from this to future date/time is ") + HobString(Long::toString((long)diff)));
#endif

	int daysDiff = (int)(diff / SecondsPerDay);  // number of seconds in a day
#ifndef EVOL_NDEBUG
	Debug::trace("HobDateTime", HobString("difference in days from this to future date/time is ") + HobString(Integer::toString(daysDiff)));
#endif
	return daysDiff;
}
