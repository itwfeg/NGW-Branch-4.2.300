/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HOBDATETIME_H
#define HOBDATETIME_H


#include <time.h>


#include "evolution.h"
#include "HfcObject.h"

#define AMINDICATOR			"am"
#define AMINDICATOR_CAPS	"AM"
#define PMINDICATOR			"pm"
#define PMINDICATOR_CAPS	"PM"


// The following was made consistent with the Quantum. (ninclude/date_time.h, ncode/super_text.c)
typedef enum DateFormats
{
	FormatDate_mmDDYY,			// 0 = DE 25, 02   -  CANADIAN_DATE
	FormatDate_mmmDDYY,			// 1 = Dec 25, 02  -  DOMESTIC_DATE
	FormatDate_YYMMDD,			// 2 = 02/12/25    -  NUMERIC_CANADIAN_DATE
	FormatDate_MMDDYY,			// 3 = 12/25/02    -  NUMERIC_DOMESTIC_DATE
	FormatDate_YYYYMMDD,		// 4 = 2002{12|25} -  CHINESE_DATE
	FormatDate_DDmmmYY,			// 5 = 25/Dec/02   -  SPANISH_DATE
	FormatDate_DDMMYY,			// 6 = 25/12/02    -  NUMERIC_SPANISH_DATE
	FormatDate_YYmmDD,			// 7 = 02 DE 25    -  CANADIAN_DATE_YY_MO_DD    from Quantum's date_time.h
	FormatDate_DD_mmm_YY,		// 8 = 25 DEC 08
	FormatDate_YYYYMMDD_dashes = 96, // New for Fastrack, for saving to Sqlite
	FormatDate_MMDDYY_NoDelimiter = 97,	// not in Quantum 122502
	FormatDate_DDD = 98,		// New for Evolution. Julian date in Q & U
	FormatDate_MMDDYYYY = 99,	// not in Quantum 12/25/2002
	FormatDate_YYMMDD_NoDelimiter = 100 // New for Fastrack, for GS1
}   DateFormats_t;

typedef enum TimeFormats
{
	FormatTime_HHMM,
	FormatTime_HHMMSS,
	FormatTime_HHMMSS_NoDelimiter
}   TimeFormats_t;

typedef enum TimeHourFormats
{
	HourFormat_AMPM,
	HourFormat_MILITARY
}   TimeHourFormats_t;

typedef enum DateTimeFormats
{
	FormatDateTime_Windux,     // MM/DD/YYYY,HH:MM:SS
	FormatDateTime_HTSeries    // YYYY-MM-DD HH:MM:SS
}   DateTimeFormats_t;



typedef enum HobDateTimeFieldOffsets
{
	fieldOffsetYear = 1,
	fieldOffsetMonth,
	fieldOffsetDay,
	fieldOffsetHour,
	fieldOffsetMinute,
	fieldOffsetHourIndicator,
	endHobDateTimeFieldOffsets
}   HobDateTimeFieldOffsets_t;

typedef enum HobDateLocales
{
	fieldOffsetLocale_EN_US,
	fieldOffsetLocale_ES_MX,
	fieldOffsetLocale_FR_CA,
	endHobDateLocaleOffsets
}   HobDateLocales_t;

class HobDateTime : public HfcObject
{
	public:

		static const long JULIAN_D0		= 1461;
		static const long JULIAN_D1		= 146097;
		static const long JULIAN_D2		= 1721119;
		static const long JULIAN_D3		= 32082;
		static const long JULIAN_D4		= 153;
		static const long JULIAN_D5		= 32044;

		static const long SecondsPerDay = 86400L;

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HobDateTime(time_t nsec = 0);
		HobDateTime(const HobDateTime &hdt);
		HobDateTime(const char *data, int len, int formatType);
		HobDateTime(long longGregDate, const short formatIndicator);
		static HobDateTime createDateTime(const std::string& date, DateFormats dateFormat, TimeFormats timeFormat = FormatTime_HHMMSS);
		static HobDateTime createDateTimeFromDBFormat(const std::string& date);

		~HobDateTime();

		static const int _UsingCurrentLocale_ = 0;
		static const int _UsingPrimaryLanguageLocale_ = 1;
		static const int _UsingSecondaryLanguageLocale_ = 2;

		// The whichLocale parameter is used to indicate which locale should be used to create the date string, where:
		// 0 = currently set locale, 1 = Primary Language locale, 2 = Secondary Language locale
		HobString formatDate(const short formatIndicator = FormatDate_MMDDYY, int usingWhichLocale = _UsingCurrentLocale_) const;
		HobString formatTime(const short formatIndicator = FormatTime_HHMMSS) const;
		HobString formatTime(const short formatIndicator, const short hoursIndicator) const;
		HobString formatDateTime(const short formatIndicator = FormatDate_MMDDYY, int usingWhichLocale = _UsingCurrentLocale_) const;
		HobString formatDateTime(const short dateFormatIndicator, const short timeFormatIndicator, int usingWhichLocale = _UsingCurrentLocale_) const;


		int getYear() const;
		int getYear2() const;
		int getYear4() const;
		void setYear(int year);

		int getMonth() const;
		void setMonth(int month);

		// Get the day-of-the-week.  Returns the number of days since Sunday;
		// i.e. Sunday = 0, ... Saturday = 6.
		int getDayOfWeek() const;
		std::string getDayOfWeekString(int usingWhichLocale = _UsingCurrentLocale_);
		std::string getDayOfWeekShortString(int usingWhichLocale = _UsingCurrentLocale_);

		int getDay() const;
		void setDay(int day);

		// Same as getHour24; returns the hour in 24-hour format (0 through 23 inclusive).
		int getHour() const;
		// Returns the hour in 12-hour format (1 through 12 inclusive).
		int getHour12() const;
		// Returns the hour in 24-hour format (0 through 23 inclusive).
		int getHour24() const;
		void setHour(int hour);

		int getMinute() const;
		void setMinute(int minute);

		int getSecond() const;
		void setSecond(int second);

		int getHourIndicator() const;
		void setHourIndicator(int hourIndicator);


		void setToCurrent();
		void setToDate(std::string &newDate);

		void incrementDays(int days);

		void incrementHours(int hours);

		void incrementMinutes(int minutes);

		void incrementSeconds(int seconds);

		void setTime();

		virtual const char *serialize(int format) const;

		void operator=(const HobDateTime& hdt);
		bool operator==(const HobDateTime& hdt) const;
		bool operator!=(const HobDateTime& hdt) const;

		bool operator <  (const HobDateTime& rhs) const;
		bool operator <= (const HobDateTime& rhs) const;
		bool operator >  (const HobDateTime& rhs) const { return !(*this <= rhs); }
		bool operator >= (const HobDateTime& rhs) const { return !(*this < rhs); }

		void show(struct tm *timeParts = (struct tm *)NULL) const;

		static short initialized;
		
		time_t getCalendarTime() const;

		void setCalendarTime(time_t time);
		time_t timeDiff(HobDateTime & time2) const;
		void setupCurrTimeParts(time_t time);

		void setDateParts(const HobString &newDate);
		void setDateParts(const HobString &newDate, const short format);
		void setDateParts(int month, int day, int year);
		void setTimeParts(const HobString &newTime, const short formatIndicator, const short hoursIndicator);
		void setTimeParts(const HobString &newTime, const short format);
		void setTimeParts(int hour, int minute, int second);
		void convertFrom(const std::string& dateTime, DateFormats dateFormat, TimeFormats timeFormat = FormatTime_HHMMSS);
		void convertFromDBFormat(const HobString *dateTime);
		void convertFromDBFormat(const std::string& dateTime);
		std::string convertToDBFormat(DateTimeFormats format = FormatDateTime_HTSeries) const;
		std::string convertToTransactionDateFormat() const;

		long getLongGregDate(const short formatIndicator);
		int getDayOfYear() const;
		long getJulianDay() const;
		void setDateFromJulianDay(long julianDay);
		bool isLeapYear() const;

		bool isValid() const;

		// The following method returns the number of days from the current date/time until the date/time of this
		// HobDateTime object.  Time of now and time of this object are irrelevant in determining the number of days.
		// For any time of today, this method will return 0.  For any time of tomorrow, this method will return 1.
		// For any time in the past, this method will return -1.
		int daysUntil(const HobDateTime &futureDateTime) const;

	private:

		time_t currTime;
		struct tm currTimeParts;

		bool valid;

		void init();
		void fixDateRollover();

		static char *twoCharMonthDescEN[13];
		static char *twoCharMonthDescES[13];
		static char *twoCharMonthDescFR[13];
		static char *threeCharMonthDescEN[13];
		static char *threeCharMonthDescES[13];
		static char *threeCharMonthDescFR[13];

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HOBDATETIME_H
