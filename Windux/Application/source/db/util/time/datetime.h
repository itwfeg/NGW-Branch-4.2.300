#ifndef DATETIME_H
#define DATETIME_H



#include "date.h"
#include "dbtime.h"



#ifndef NO_NAMESPACES
namespace util_time
{
#endif


	class DateTime
	{
		public:
			// --- Types and subtypes ---
			class Interval
			{
				friend class DateTime;

				public:
					Interval(const Interval& ti)
							: days_(ti.days_), seconds_(ti.seconds_), milliseconds_(ti.milliseconds_) {}

					int getDays() const { return days_; }
					int getTotalDays() const { return days_; }
					int getSeconds() const { return seconds_; }
					int getTotalSeconds() const { return days_ * 86400 + seconds_; }
					int getMilliseconds() const { return milliseconds_; }
					int getTotalMilliseconds() const { return getTotalSeconds() * 1000 + milliseconds_; }

				protected:
					Interval(int days, int seconds = 0, int milli = 0);

					void setInterval(int days, int seconds, int milli) 
							{ days_ = days; seconds_ = seconds; milliseconds_ = milli; }

				private:
					int days_;
					int seconds_;
					int milliseconds_;

					// Do not allow the default assignment operator.
					Interval operator=(const Interval& rhs);
			};

			typedef Date::Weekday Weekday;
			typedef Date::Month Month;


			// --- Constructors and destructor ---
			DateTime();
			DateTime(const std::string& str);
			DateTime(const Date& date, const Time& time = Time(0, 0));
			// Months are in the [1:12] range, while month days are in [1:31]
			DateTime(int year, int month, int monthday);
			DateTime(const DateTime& rhs);
			~DateTime();


			// --- Accessor methods ---
			Date date_;
			Time time_;


			short getYear() const { return date_.getYear(); }
			void setYear(int year) { date_.setYear(year); }
			void setYear(short year) { date_.setYear(year); }

			// Months are in the range [1:12]
			Month getMonth() const { return date_.getMonth(); }
			void setMonth(Month month) { date_.setMonth(month); }
			void setMonth(int month) { date_.setMonth(month); }

			// Weekdays are in the range [sun:sat]
			Weekday getWeekDay() const { return date_.getWeekDay(); }

			// Month days are in the range [1:31]
			short getMonthDay() const { return date_.getMonthDay(); }
			void setMonthDay(int day) { date_.setMonthDay(day); }
			void setMonthDay(short day) { date_.setMonthDay(day); }

			short getHour() const { return time_.getHour(); }
			void setHour(int hour) { time_.setHour(hour); }
			void setHour(short hour) { time_.setHour(hour); }

			short getMinute() const { return time_.getMinute(); }
			void setMinute(int minute) { time_.setMinute(minute); }
			void setMinute(short minute) { time_.setMinute(minute); }

			short getSecond() const { return time_.getSecond(); }
			void setSecond(int second) { time_.setSecond(second); }
			void setSecond(short second) { time_.setSecond(second); }

			short getMillisecond() const { return time_.getMillisecond(); }
			void setMillisecond(int milli) { time_.setMillisecond(milli); }
			void setMillisecond(short milli) { time_.setMillisecond(milli); }

			bool isValid() const { return date_.isValid(); }

			// Months are in the range [1:12], while month days are in the range [1:31]
			void setDate(short year, short month, short monthday)
					{ date_.setDate(year, month, monthday); }
			void setTime(int hour, int minute = 0, int second = 0, int milli = 0)
					{ time_.setTime(hour, minute, second, milli); }


			// --- Processing methods ---

			void getCurrentDateTime();

			
			// Logical operators
			bool operator <  (const DateTime& dt) const;
			bool operator <= (const DateTime& dt) const;
			bool operator >  (const DateTime& dt) const;
			bool operator >= (const DateTime& dt) const;
			bool operator == (const DateTime& dt) const;
			bool operator != (const DateTime& dt) const;


			// Arithmetic and assignment operators
			DateTime operator + (int days) const;
			DateTime operator - (int days) const;
			Interval operator - (const DateTime& dt) const;
			DateTime& operator = (const DateTime& dt);
			DateTime& operator += (int days);
			DateTime& operator -= (int days);
			DateTime& operator ++ ();
			DateTime operator ++ (int);
			DateTime& operator -- ();
			DateTime operator -- (int);


			// Conversion operators
			operator std::string() const;
			std::string standardLook() const;


		private:
			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace util_time
#endif



#include <ostream>
#ifndef NO_NAMESPACES
namespace std
{
#endif


#ifndef NO_NAMESPACES
	using util_time::DateTime;
#endif


	ostream& operator<<(ostream& os, const DateTime& dt);


#ifndef NO_NAMESPACES
}
#endif



#endif // DATETIME_H
