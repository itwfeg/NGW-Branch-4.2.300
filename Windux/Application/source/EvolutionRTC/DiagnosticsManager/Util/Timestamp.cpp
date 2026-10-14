/**
 * @file Timestamp.cpp
 * @brief Timestamp class in Unix Epoch Milliseconds.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "Timestamp.h"

#include <Configuration.h>

#include <e_string/utilstring.h>

#if defined( _MSC_VER )
#include <stdint.h>
#include <time.h>
#elif defined( __GNUC__ )
#include <sys/time.h>
#endif

/// Util objects namespace.
namespace util
{

///////////////////////////////////////////////////////////////////////////////////////////////////

#if defined( _MSC_VER )
int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
Timestamp::Timestamp( const std::string &value )
{
	value_ = util_string::stringtolonglong( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Timestamp & Timestamp::operator = ( const std::string &lhs )
{
	value_ = util_string::stringtolonglong( lhs );
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Timestamp::addSeconds( int value )
{
	// adjust value
	long long convert( 1000 );
	convert *= value;

	value_ += convert;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Timestamp::addMinutes( int value )
{
	// adjust value
	long long convert( 60 * 1000 );
	convert *= value;

	value_ += convert;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Timestamp::addHours( int value )
{
	// adjust value
	long long convert( 60 * 60 * 1000 );
	convert *= value;

	value_ += convert;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Timestamp::addDays( int value )
{
	// adjust value
	long long convert( 24 * 60 * 60 * 1000 );
	convert *= value;

	value_ += convert;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Timestamp::fromDateTime( const std::string &value )
{
	// 0         1         2
	// 01234567890123456789012345
	// MM/DD/YYYY HH:MM:SS.sss
	if ( 10 <= value.length() )
		if (( '/' == value[2] ) && ( '/' == value[5] ))
		{
			struct tm t;
			memset( &t, 0, sizeof(t) );

			t.tm_year = util_string::stringtolong( value.substr( 6, 4 ) ) - 1900;
			t.tm_mon = util_string::stringtolong( value.substr( 0, 2 ) ) - 1;
			t.tm_mday = util_string::stringtolong( value.substr( 3, 2 ) );

			if (( 16 <= value.length() ) && ( ':' == value[13] ))
			{
				t.tm_hour = util_string::stringtolong( value.substr( 11, 2 ) );
				t.tm_min = util_string::stringtolong( value.substr( 14, 2 ) );
			}

			if (( 19 <= value.length() ) && ( ':' == value[16] ))
				t.tm_min = util_string::stringtolong( value.substr( 17, 2 ) );

			value_ = mktime( &t );
			value_ *= 1000;

			if (( 23 <= value.length() ) && ( '.' == value[19] ))
				value_ += util_string::stringtolong( value.substr( 20, 3 ) );
		}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string Timestamp::toString() const
{
	return util_string::longlongtostring( value_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Timestamp::now( long long &value )
{
	struct timeval tp;
	gettimeofday( &tp, NULL );

	// set value
	value = tp.tv_sec;
	value *= 1000;
	value += (tp.tv_usec / 1000);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string Timestamp::formatDateTime( long long timestamp )
{
	std::string format;

	switch ( Configuration::getInstance()->getDateDisplayFormat() )
	{
	case FormatDate_mmDDYY:				// 0 = DE 25, 02   -  CANADIAN_DATE
	case FormatDate_mmmDDYY:			// 1 = Dec 25, 02  -  DOMESTIC_DATE
		format += "%b %d, %y";
		break;
	case FormatDate_YYMMDD:				// 2 = 02/12/25    -  NUMERIC_CANADIAN_DATE
		format += "%y/%m/%d";
		break;
	case FormatDate_YYYYMMDD:			// 4 = 2002{12|25} -  CHINESE_DATE
		format += "%Y{%m|%d}";
		break;
	case FormatDate_DDmmmYY:			// 5 = 25/Dec/02   -  SPANISH_DATE
		format += "%d/%b/%y";
		break;
	case FormatDate_DDMMYY:				// 6 = 25/12/02    -  NUMERIC_SPANISH_DATE
		format += "%d/%m/%y";
		break;
	case FormatDate_YYmmDD:				// 7 = 02 DE 25    -  CANADIAN_DATE_YY_MO_DD    from Quantum's date_time.h
		format += "%y %b %d";
		break;
	case FormatDate_DD_mmm_YY:			// 8 = 25 DEC 08
		format += "%d %b %y";
		break;
	case FormatDate_MMDDYY:				// 3 = 12/25/02    -  NUMERIC_DOMESTIC_DATE
	default:
		format += "%m/%d/%y";
		break;
	}

	format += " ";

	switch ( Configuration::getInstance()->getTimeDisplayFormat() )
	{
	case HourFormat_MILITARY:
		format += "%H:%M:%S";
		break;
	case HourFormat_AMPM:
	default:
		format += "%I:%M:%S %p";
		break;
	}

	// convert timestamp to text
	char stamp[512];

	long long t( timestamp / 1000 );
	time_t ts( t );

	strftime( stamp, sizeof(stamp), format.c_str(), localtime( &ts ) );

	return std::string( stamp );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace
