#include "PAL_platformheader.h"

#include "date.h"


// Tropical year = 365.242199 days
// Every 4 years add 1 day
// Every 100 years subtract 1 day
// Every 400 years add 1 day
// (Proposed) Every 4000 years subtract 1 day

#if defined ( PAL_WIN32_RUN )
#  include <sstream>
#  include <iomanip>
#  include "..\platform\platform.h"
#else
#  include <cstdio>
#ifdef LINUX_FIX
#  include <sstream>
#endif
#  include "../platform/platform.h"
#endif


#ifndef NO_NAMESPACES
namespace util_time
{
#endif


#ifndef NO_NAMESPACES
using util_platform::Platform;
#endif



Date::Date(int hobartDate)
{
	/* (SJJC port 6/19/06) */
	month_ = (short)(hobartDate / 10000 - 1);          // Month's range is [0-11]
	monthday_ = (short)((hobartDate % 10000) / 100);
	year_ = (short)(hobartDate % 100);
	numdays_ = 0;

	validateDate();
}


Date::Date(const std::string& date)
{
	char c;
	std::istringstream dateStream(date);

	dateStream >> year_ >> c >> month_ >> c >> monthday_;

	--month_;
	numdays_ = 0;

	validateDate();
}


void Date::getCurrentDate()
{
	Platform::getInstance().getCurrentDate(*this);
}


bool Date::operator <(const Date& dt) const
{
	if ( this->year_ == dt.year_ )
	{
		if ( this->month_ == dt.month_ )
		{
			if ( this->monthday_ == dt.monthday_ )
				return false;
			else
				return this->monthday_ < dt.monthday_;
		}
		else
			return this->month_ < dt.month_;
	}
	else
		return this->year_ < dt.year_;
}


bool Date::operator <= (const Date& dt) const
{
	return !(dt < *this);
}


bool Date::operator >  (const Date& dt) const
{
	return (dt < *this);
}


bool Date::operator >= (const Date& dt) const
{
	return !(*this < dt);
}


bool Date::operator == (const Date& dt) const
{
	return ( this->monthday_ == dt.monthday_ && this->month_ == dt.month_
			&& this->year_ == dt.year_ );
}


bool Date::operator != (const Date& dt) const
{
	return !(*this == dt);
}


Date Date::operator +(int days) const
{
	Date dt(*this);

	dt.numdays_ += days;

	if ( !dt.validateDays() )
		throw InvalidDate("Invalid Date");

	dt.daysToCalendar();

	return dt;
}


Date Date::operator -(int days) const
{
	return *this + (-days);
}


int Date::operator - (const Date& dt) const
{
	return this->numdays_ - dt.numdays_;
}


Date& Date::operator = (const Date& dt)
{
	year_ = dt.year_;
	month_ = dt.month_;
	monthday_ = dt.monthday_;
	numdays_ = dt.numdays_;

	return *this;
}


Date& Date::operator += (int days)
{
	numdays_ += days;
	if ( !validateDays() )
		throw InvalidDate("Invalid Date");

	daysToCalendar();

	return *this;
}


Date& Date::operator -= (int days)
{
	return this->operator+=(-days);
}


Date& Date::operator ++ ()
{
	++numdays_;

	daysToCalendar();

	return *this;
}


Date Date::operator ++ (int)
{
	Date dt(*this);

	this->operator++();

	return dt;
}


Date& Date::operator -- ()
{
	--numdays_;
	if ( !validateDays() )
		throw InvalidDate("Invalid Date");

	daysToCalendar();

	return *this;
}


Date Date::operator -- (int)
{
	Date dt(*this);

	this->operator--();

	return dt;
}


Date::operator std::string() const
{
#ifndef VXWORKS
/*	std::ostringstream s;

	s.setf(std::ios_base::fixed, std::ios_base::floatfield);
	s << year_ << "/" << (month_ + 1) << "/" << monthday_;

	return s.str(); */
	char buffer[22];

	sprintf(buffer, "%d/%d/%d", year_, month_ + 1, monthday_);

	return buffer;
#else
	char buffer[33];

	sprintf(buffer, "%d/%d/%d", year_, month_ + 1, monthday_);

	return buffer;
#endif
}


std::string Date::standardLook() const
{
#ifndef VXWORKS
	std::ostringstream s;

	s << (month_ + 1) << "/" << monthday_ << "/" << year_;

	return s.str();
#else
	char buffer[33];

	sprintf(buffer, "%d/%d/%d", month_ + 1, monthday_, year_);

	return buffer;
#endif
}


std::string Date::hobartLook() const
{
#ifndef VXWORKS
	std::ostringstream s;

	if ( year_ < 10 )
		s << getMonthName(month_ + 1) << " " << monthday_ << ", 0" << year_;
	else
		s << getMonthName(month_ + 1) << " " << monthday_ << ", " << year_;

	return s.str();
#else
	char buffer[33];

	if ( year_ < 10 )
		sprintf(buffer, "%s %d, %2.2d", getMonthName(month_ + 1), monthday_, year_);
	else
		sprintf(buffer, "%s %d, %d", getMonthName(month_ + 1), monthday_, year_);

	return buffer;
#endif
}


void Date::validateDate()
{
	bool valid = true;

	if ( month_ < 0 )
	{
		year_ = (short)(year_ + (month_ / 12 - 1));	 /* (SJJC port 6/19/06) */
		month_ = (short)(month_ % 12 + 12);
	}
	else if ( month_ >= 12 )
	{
		year_ = (short)(year_ + (month_ / 12));	 /* (SJJC port 6/19/06) */
		month_ %= (short)12;
	}

	if ( year_ <= 0 )
		valid = false;

	if ( valid )
	{
		calculateDays();

		if ( validateDays() )
			daysToCalendar();
		else
			valid = false;
	}

	if ( !valid )
		throw InvalidDate("Invalid Date");
}


bool Date::isLeapYear(int year)
{
	if ( year % 4 )
		return false;
	if ( year % 100 )
		return true;
	if ( year % 400 )
		return false;

	return true;
}


// The assumption is that the month and the year are within their ranges.
// That needs to be validated before getting into this method.
// The basis for this computation is that the first year is 1 not 0.
void Date::calculateDays()
{
	int year = year_ - 1;

	// Number of days owed to the number of blocks of 400 years.
	numdays_ = year / 400 * _daysIn400Years_;
	year %= 400;
	// Add the number of days owed to the number of blocks of 100 years in
	// the last block of 400
	numdays_ += year / 100 * _daysIn100Years_;
	year %= 100;
	// Add the number of days owed to the number of blocks of 4 years in the
	// last block of 100 plus the number of years in the last block of 4 years
	numdays_ += year / 4 * _daysIn4Years_ + _yearDaysIn4Years_[year % 4];

	// Add the number of days owed to the number of months already elapsed
	// in the current year
	if ( isLeapYear(year_) )
		numdays_ += _monthDaysInLeapYear_[month_];
	else
		numdays_ += _monthDaysInYear_[month_];

	// Add the number of days elapsed in the current month
	numdays_ += monthday_ - 1;
}


void Date::daysToCalendar()
{
	int days = numdays_;
	// Number of years owed to 400 year blocks
	year_ = (short)(days / _daysIn400Years_ * 400);	 /* (SJJC port 6/19/06) */
	days %= _daysIn400Years_;
	// Number of years owed to 100 year blocks
	year_ = (short)(year_ + (days / _daysIn100Years_ * 100));  /* (SJJC port 6/19/06) */
	days %= _daysIn100Years_;
	// Number of years owed to 4 year blocks
	year_ = (short)(year_ + (days / _daysIn4Years_ * 4));  /* (SJJC port 6/19/06) */
	days %= _daysIn4Years_;

	int index = calculateIndex(_yearDaysIn4Years_, 4, days);
	year_ = (short)(year_ + index);
	days -= _yearDaysIn4Years_[index - 1];
	if ( isLeapYear(year_) )
	{
		int index2 = calculateIndex(_monthDaysInLeapYear_, 12, days);
		month_ = (short)(index2 - 1);
		days -= _monthDaysInLeapYear_[index2 - 1];
	}
	else
	{
		int index2 = calculateIndex(_monthDaysInYear_, 12, days);
		month_ = (short)(index2 - 1);
		days -= _monthDaysInYear_[index2 - 1];
	}

	monthday_ = (short)(days + 1);
}


int Date::calculateIndex(int array[], int total, int number)
{
	int index = 0;
	while ( index < total )
	{
		if ( number < array[index] )
			return index;

		++index;
	}

	return index;
}


	const char* Date::getMonthName(int month)
	{
		switch ( month )
		{
			case jan:
				return "Jan";
			case feb:
				return "Feb";
			case mar:
				return "Mar";
			case apr:
				return "Apr";
			case may:
				return "May";
			case jun:
				return "Jun";
			case jul:
				return "Jul";
			case aug:
				return "Aug";
			case sep:
				return "Sep";
			case oct:
				return "Oct";
			case nov:
				return "Nov";
			case dec:
				return "Dec";
		}

		return "";
	}



int Date::_daysIn400Years_ = 146097;
int Date::_daysIn100Years_ = 36524;
int Date::_daysIn4Years_ = 1461;
int Date::_yearDaysIn4Years_[] = { 0, 365, 730, 1095 };
int Date::_monthDaysInLeapYear_[] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
int Date::_monthDaysInYear_[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
int Date::_firstDayInCalendar_ = 577735;           // 10/15/1582
int Date::_firstWeekDayInCalendar_ = 1;



#ifndef NO_NAMESPACES
} // namespace util_time
#endif



#include <ostream>
#include <iomanip>
#ifndef NO_NAMESPACES
namespace std
{
#endif


#ifndef NO_NAMESPACES
	using util_time::Date;
#endif


	ostream& operator<<(ostream& os, const Date& dt)
	{
		return os << setw(2) << setfill('0') << dt.getMonth() << "/"
				<< setw(2) << setfill('0') << dt.getMonthDay() << "/"
				<< setw(4) << setfill('0') << dt.getYear();
	}



#ifndef NO_NAMESPACES
}
#endif

