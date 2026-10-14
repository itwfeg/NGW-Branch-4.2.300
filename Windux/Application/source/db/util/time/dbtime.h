#ifndef DBTIME_H
#define DBTIME_H



#include <string>
#ifdef WIN32_OS
	#include <exception>
#else
	#include <stdexcept>
#endif


#ifndef NO_NAMESPACES
namespace util_time
{
#endif


	class Time
	{
		public:
			// --- Types and subtypes ---
			class Interval
			{
				friend class Time;

				public:
					Interval(const Interval& ti) : seconds_(ti.seconds_), milliseconds_(ti.milliseconds_) {}

					int getSeconds() const { return seconds_; }
					int getTotalSeconds() const { return seconds_; }
					int getMilliseconds() const { return milliseconds_; }
					int getTotalMilliseconds() const { return seconds_ * 1000 + milliseconds_; }

				protected:
					Interval(int seconds = 0, int milli = 0) : seconds_(seconds), milliseconds_(milli)
							{ if ( seconds_ < 0 ) seconds_ += 86400; if ( milliseconds_ < 0 ) { milliseconds_ += 1000; --seconds_; } }

					void setInterval(int seconds, int milli) { seconds_ = seconds; milliseconds_ = milli; }

				private:
					int seconds_;
					int milliseconds_;

					// Do not allow the default assignment operator.
					Interval operator=(const Interval& rhs);
			};


			// --- Constructors and destructor ---
			Time() : hour_(0), minute_(0), second_(0), millisecond_(0) { getCurrentTime(); }
			Time(int hour, int minute, int second = 0, int millisecond = 0)
					: hour_((short)(hour)), minute_((short)(minute)),
					second_((short)(second)), millisecond_((short)(millisecond))
					{ validateTime(); }
			Time(const std::string& time);
			~Time() {}


			// --- Accessor methods ---
			short getHour() const { return hour_; }
			void setHour(int hour) { hour_ = (short) hour; }
			void setHour(short hour) { hour_ = hour; }

			short getMinute() const { return minute_; }
			void setMinute(int minute) { minute_ = (short) minute; }
			void setMinute(short minute) { minute_ = minute; }

			short getSecond() const { return second_; }
			void setSecond(int second) { second_ = (short) second; }
			void setSecond(short second) { second_ = second; }

			int getTotalSeconds() const { return hour_ * 3600 + minute_ * 60 + second_; }

			short getMillisecond() const { return millisecond_; }
			void setMillisecond(int milli) { millisecond_ = (short) milli; }
			void setMillisecond(short milli) { millisecond_ = milli; }

			void setTime(int hour, int minute = 0, int second = 0, int milli = 0)
					{ hour_ = (short)hour; minute_ = (short)minute;
					second_ = (short)second; millisecond_ = (short)milli; validateTime(); }


			// --- Processing methods ---

			void getCurrentTime();

			
			// Logical operators
			bool operator <  (const Time& t) const;
			bool operator <= (const Time& t) const;
			bool operator >  (const Time& t) const;
			bool operator >= (const Time& t) const;
			bool operator == (const Time& t) const;
			bool operator != (const Time& t) const;


			// Arithmetic and assignment operators
			Interval operator - (const Time& dt) const;
			Time& operator = (const Time& dt);


			// Conversion operators
			operator std::string() const;
			std::string standardLook() const;


		private:
			// --- Data members ---
			short hour_;               // Range [0:23]
			short minute_;             // Range [0:59]
			short second_;             // Range [0:59]
			short millisecond_;        // Range [0:999]

			void validateTime();
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
using namespace util_time;
#endif


	ostream& operator<<(ostream& os, const Time& t);


#ifndef NO_NAMESPACES
}
#endif



#endif // DBTIME_H
