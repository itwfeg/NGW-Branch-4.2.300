#ifndef TRANSACTIONDATESMESSAGE_H
#define TRANSACTIONDATESMESSAGE_H



#include "HfcObject.h"


#include "HobList.h"
#include "CSmartPtr.h"


namespace hfc
{


	class Transaction;


	class TransactionDatesMessage : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
			TransactionDatesMessage();
			~TransactionDatesMessage();


			/* --- Getters and setters --- */


			/* --- Instance function members --- */
			void addWeekDay(int weekDay);
			void addDateDisplay(const std::string& date);
			void addDate(const std::string& date);

			const char * serialize(int format) const;


			/* --- Class function members --- */


		private:
			/* --- Data members --- */
			HobList weekDays_;
			HobList datesDisplay_;
			HobList dates_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif

			/* --- Constructors --- */


			/* --- Instance function members --- */
	};


}



#endif /* TRANSACTIONDATESMESSAGE_H */
