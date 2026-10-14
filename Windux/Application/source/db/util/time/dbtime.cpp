#include "PAL_platformheader.h"

#ifdef VXWORKS
	#include <cstdio>
#else
	#include <sstream>
#endif



#include "dbtime.h"



#if defined ( PAL_WIN32_RUN )
	#include "..\platform\platform.h"
#else
	#include "../platform/platform.h"
#endif


#ifndef NO_NAMESPACES
namespace util_time
{
#endif


#ifndef NO_NAMESPACES
using util_platform::Platform;
#endif


Time::Time(const std::string& time)
{
	char c;
	std::istringstream timeStream(time);

	timeStream >> hour_ >> c >> minute_;
	
	if ( timeStream >> c )
	{
		timeStream >> second_;

		if ( timeStream >> c )
			timeStream >> millisecond_;
		else
			millisecond_ = 0;
	}
	else
	{
		second_ = 0;
		millisecond_ = 0;
	}

	validateTime();
}


void Time::getCurrentTime()
{
	Platform::getInstance().getCurrentTime(*this);
}


bool Time::operator <(const Time& t) const
{
	if ( this->hour_ == t.hour_ )
	{
		if ( this->minute_ == t.minute_ )
		{
			if ( this->second_ == t.second_ )
			{
				if ( this->millisecond_ == t.millisecond_ )
					return false;
				else
					return this->millisecond_ < t.millisecond_;
			}
			else
				return this->second_ < t.second_;
		}
		else
			return this->minute_ < t.minute_;
	}
	else
		return this->hour_ < t.hour_;
}


bool Time::operator <= (const Time& t) const
{
	return !(t < *this);
}


bool Time::operator >  (const Time& t) const
{
	return (t < *this);
}


bool Time::operator >= (const Time& t) const
{
	return !(*this < t);
}


bool Time::operator == (const Time& t) const
{
	if ( this->second_ == t.second_ && this->minute_ == t.minute_ && this->hour_ == t.hour_
			&& this->millisecond_ == t.millisecond_ )
		return true;
	else
		return false;
}


bool Time::operator != (const Time& t) const
{
	return !(*this == t);
}


Time::Interval Time::operator - (const Time& t) const
{
	// 'this' is the later time therefore the total seconds associated with 'this' must
	// be greater or equal to the totals seconds associated with 't'. However, the number
	// of milliseconds could vary randomly as long as if the milliseconds associated with
	// 't' are greater than the ones associated with 'this' then the number of seconds
	// associated with 'this' must be strictly greater than the ones associated with 't'.
	return Interval(getTotalSeconds() - t.getTotalSeconds(), millisecond_ - t.millisecond_);
}


Time& Time::operator = (const Time& t)
{
	hour_ = t.hour_;
	minute_ = t.minute_;
	second_ = t.second_;
	millisecond_ = t.millisecond_;

	return *this;
}


Time::operator std::string() const
{
#ifndef VXWORKS
	std::ostringstream s;

	s << hour_ << ":" << minute_ << ":" << second_ << ":" << millisecond_;

	return s.str();
#else
	char buffer[33];

	sprintf(buffer, "%d:%02d:%02d:%03d", hour_, minute_, second_, millisecond_);

	return buffer;
#endif
}


std::string Time::standardLook() const
{
#ifndef VXWORKS
	std::ostringstream s;

	s << hour_ << ":" << minute_ << ":" << second_;

	return s.str();
#else
	char buffer[33];

	if ( hour_ <= 12 )
		sprintf(buffer, (hour_ == 12 ? "%d:%02d:%02d PM" : "%d:%02d:%02d AM"), hour_, minute_, second_);
	else
		sprintf(buffer, "%d:%02d:%02d PM", hour_ - 12, minute_, second_);

	return buffer;
#endif
}


void Time::validateTime()
{
	// Validate milliseconds
	second_ = (short)(second_ + millisecond_ / 1000);  /* (SJJC port 6/19/06) */
	millisecond_ %= (short)(1000);

	// Validate seconds
	minute_ = (short)(minute_ + second_ / 60);	/* (SJJC port 6/19/06) */
	second_ %= (short)60;

	// Validate minutes
	hour_ = (short)(hour_ + minute_ / 60);	/* (SJJC port 6/19/06) */
	minute_ %= (short)60;

	// Validate hours
	hour_ %= (short)24;
}





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
	using util_time::Time;
#endif


	ostream& operator<<(ostream& os, const Time& t)
	{
		return os << setw(2) << setfill('0') << t.getHour() << ":"
				<< setw(2) << setfill('0') << t.getMinute() << ":"
				<< setw(2) << setfill('0') << t.getSecond();
	}

	
#ifndef NO_NAMESPACES
}
#endif

