#include "PAL_platformheader.h"

#include "datetime.h"


// Tropical year = 365.242199 days
// Every 4 years add 1 day
// Every 100 years subtract 1 day
// Every 400 years add 1 day
// (Proposed) Every 4000 years subtract 1 day

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

#if defined ( PAL_WIN32_RUN )
#  include <sstream>
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


	using namespace std;
	#ifndef NO_NAMESPACES
	using util_platform::Platform;
	#endif


	#ifdef USE_ACCOUNTANT
		int DateTime::_dataType_ = 0;
		string DateTime::_dataTypeName_("util_time::DateTime");
	#endif


	DateTime::DateTime()
		: date_(), time_()
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	DateTime::DateTime(const std::string& str)
		: date_(), time_()
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		string::size_type separator = str.find(',');
		if ( separator != string::npos )
		{
			date_ = Date(str.substr(0, separator));
			time_ = Time(str.substr(separator + 1));
		}
		else
		{
			separator = str.find('/');
			if ( separator != string::npos )
				date_ = Date(str);
			else
				time_ = Time(str);
		}
	}


	DateTime::DateTime(const Date& date, const Time& time)
		: date_(date), time_(time)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	// Months are in the [1:12] range, while month days are in [1:31]
	DateTime::DateTime(int year, int month, int monthday)
		: date_(year, month, monthday), time_()
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	DateTime::DateTime(const DateTime& rhs)
		: date_(rhs.date_), time_(rhs.time_)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	DateTime::~DateTime()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}

	
void DateTime::getCurrentDateTime()
{
	*this = Platform::getInstance().getCurrentDateTime();
}


bool DateTime::operator <(const DateTime& dt) const
{
	if ( this->date_ == dt.date_ )
	{
		if ( this->time_ == dt.time_ )
			return false;
		else
			return this->time_ < dt.time_;
	}
	else
		return this->date_ < dt.date_;
}


bool DateTime::operator <= (const DateTime& dt) const
{
	return !(dt < *this);
}


bool DateTime::operator >  (const DateTime& dt) const
{
	return (dt < *this);
}


bool DateTime::operator >= (const DateTime& dt) const
{
	return !(*this < dt);
}


bool DateTime::operator == (const DateTime& dt) const
{
	return ( this->date_ == dt.date_ && this->time_ == dt.time_ );
}


bool DateTime::operator != (const DateTime& dt) const
{
	return !(*this == dt);
}


DateTime DateTime::operator +(int days) const
{
	DateTime dt(*this);

	dt.date_ += days;

	return dt;
}


DateTime DateTime::operator -(int days) const
{
	return *this + (-days);
}


DateTime::Interval DateTime::operator - (const DateTime& dt) const
{
	// 'this' is the later datetime, however 'seconds1' may be smaller than 'seconds2'
	// if between 't' and 'this' we crossed over a date boundary (possibly multiple ones).
	// This would happen if the dates are different.
	return Interval(date_ - dt.date_, time_.getTotalSeconds() - dt.time_.getTotalSeconds(), time_.getMillisecond() - dt.time_.getMillisecond());
}


DateTime& DateTime::operator = (const DateTime& dt)
{
	date_ = dt.date_;
	time_ = dt.time_;

	return *this;
}


DateTime& DateTime::operator += (int days)
{
	date_ += days;

	return *this;
}


DateTime& DateTime::operator -= (int days)
{
	return this->operator+=(-days);
}


DateTime& DateTime::operator ++ ()
{
	++date_;

	return *this;
}


DateTime DateTime::operator ++ (int)
{
	DateTime dt(*this);

	++this->date_;

	return dt;
}


DateTime& DateTime::operator -- ()
{
	--date_;

	return *this;
}


DateTime DateTime::operator -- (int)
{
	DateTime dt(*this);

	--date_;

	return dt;
}


DateTime::operator std::string() const
{
#ifndef VXWORKS
	std::ostringstream s;

	s << date_ << "," << time_;

	return s.str();
#else
	char buffer[33];

	std::sprintf(buffer, "%d/%d/%d, %d:%02d:%02d:%03d", date_.getYear(), date_.getMonth(), date_.getMonthDay(),
			time_.getHour(), time_.getMinute(), time_.getSecond(), time_.getMillisecond());

	return buffer;
#endif
}


string DateTime::standardLook() const
{
#ifndef VXWORKS
	std::ostringstream s;

	s << date_.standardLook() << ", " << time_.standardLook();

	return s.str();
#else
	char buffer[33];

	if ( time_.getHour() <= 12 )
		std::sprintf(buffer, (time_.getHour() == 12 ? "%d/%d/%d, %d:%02d:%02d PM" : "%d/%d/%d, %d:%02d:%02d AM"),
				date_.getMonth(), date_.getMonthDay(), date_.getYear(),
				time_.getHour(), time_.getMinute(), time_.getSecond());
	else
		std::sprintf(buffer, "%d/%d/%d, %d:%02d:%02d PM", date_.getMonth(), date_.getMonthDay(), date_.getYear(),
				time_.getHour() - 12, time_.getMinute(), time_.getSecond());

	return buffer;
#endif
}


DateTime::Interval::Interval(int days, int seconds, int milli)
		: days_(days), seconds_(seconds), milliseconds_(milli)
{
	if ( milliseconds_ < 0 )
	{
		milliseconds_ += 1000;
		--seconds_;
	}

	if ( seconds_ < 0 )
	{
		seconds_ += 86400;
		--days_;
	}
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
	using util_time::DateTime;
#endif


	ostream& operator<<(ostream& os, const DateTime& dt)
	{
		return os << setw(2) << setfill('0') << dt.getMonth() << "/"
				<< setw(2) << setfill('0') << dt.getMonthDay() << "/"
				<< setw(4) << setfill('0') << dt.getYear() << " "
				<< setw(2) << setfill('0') << dt.getHour() << ":"
				<< setw(2) << setfill('0') << dt.getMinute() << ":"
				<< setw(2) << setfill('0') << dt.getSecond();
	}


#ifndef NO_NAMESPACES
}
#endif

