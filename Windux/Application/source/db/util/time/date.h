#ifndef DATE_H
#define DATE_H



#include <string>
#include <stdexcept>



#ifndef NO_NAMESPACES
namespace util_time
{
#endif


	using std::logic_error;


	class Date
	{
		public:
			// --- Types and subtypes ---
			enum Weekday {sun, mon, tue, wed, thu, fri, sat};
			enum Month {jan = 1, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec};
			class InvalidDate : public logic_error       // Exception class
			{
				public:
					InvalidDate(const std::string& message) : logic_error(message) {}
			};


			// --- Constructors and destructor ---
			Date() : year_(1), month_(jan - 1), monthday_(1), numdays_(0) { getCurrentDate(); }
			// Months are in the range [1:12], while month days are in the range [1:31]
			Date(int year, int month, int monthday) : year_((short)year), 
					month_((short)((month - 1)%12)), monthday_((short)monthday), numdays_(0)
					{ validateDate(); }
			Date(int hobartDate);
			Date(const std::string& date);
			~Date() {}


			// --- Accessor methods ---
			short getYear() const { return year_; }
			void setYear(int year) { year_ = (short) year; validateDate(); }
			void setYear(short year) { year_ = year; validateDate(); }

			// Months are in the range [1:12]
			Month getMonth() const { return Month(month_ + 1); }
			void setMonth(Month month) { month_ = (short)(month - 1); validateDate(); }
			void setMonth(int month) { month_ = (short)((month - 1)%12); validateDate(); }

			// Weekdays are in the range [sun:sat]
			Weekday getWeekDay() const { return Weekday((numdays_ + _firstWeekDayInCalendar_)%7); }

			// Month days are in the range [1:31]
			short getMonthDay() const { return monthday_; }
			void setMonthDay(int day) { monthday_ = (short) day; validateDate(); }
			void setMonthDay(short day) { monthday_ = day; validateDate(); }

			bool isValid() const { return year_ > 0; }

			// Months are in the range [1:12], while month days are in the range [1:31]
			void setDate(short year, short month, short monthday)
					{ year_ = year; month_ = (short)((month - 1)%12); monthday_ = monthday; validateDate(); }


			// --- Processing methods ---

			void getCurrentDate();

			
			// Logical operators
			bool operator <  (const Date& dt) const;
			bool operator <= (const Date& dt) const;
			bool operator >  (const Date& dt) const;
			bool operator >= (const Date& dt) const;
			bool operator == (const Date& dt) const;
			bool operator != (const Date& dt) const;


			// Arithmetic and assignment operators
			Date operator + (int days) const;
			Date operator - (int days) const;
			int operator - (const Date& dt) const;
			Date& operator = (const Date& dt);
			Date& operator += (int days);
			Date& operator -= (int days);
			Date& operator ++ ();
			Date operator ++ (int);
			Date& operator -- ();
			Date operator -- (int);


			// Conversion operators
			operator std::string() const;
			std::string standardLook() const;
			std::string hobartLook() const;


		private:
			// --- Instance Data members ---
			short year_;               // Range [1:~)
			short month_;              // Range [0:11]
			short monthday_;           // Range [1:31]
			mutable int numdays_;	   // Range [0:~)

			// --- Class Data members ---
			static int _daysIn400Years_;
			static int _daysIn100Years_;
			static int _daysIn4Years_;
			static int _yearDaysIn4Years_[4];
			static int _monthDaysInLeapYear_[12];
			static int _monthDaysInYear_[12];
			static int _firstDayInCalendar_;
			static int _firstWeekDayInCalendar_;

			// --- Instance Function members ---
			void validateDate();
			bool isLeapYear(int year);
			void calculateDays();
			bool validateDays() { return numdays_ >= _firstDayInCalendar_; }
			void daysToCalendar();
			int calculateIndex(int array[], int total, int number);

			// --- Class Function members ---
			static const char* getMonthName(int month);
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
	ostream& operator<<(ostream& os, const util_time::Date& dt);
#else
	ostream& operator<<(ostream& os, const Date& dt);
#endif


#ifndef NO_NAMESPACES
}
#endif



#endif // DATE_H
