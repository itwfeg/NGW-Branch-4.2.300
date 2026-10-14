#ifndef READFILTEREDHOURLYTOTALSMESSAGE_H
#define READFILTEREDHOURLYTOTALSMESSAGE_H



#include "HfcObject.h"



namespace hfc
{


	class ReadFilteredHourlyTotalsMessage : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum ReadFilteredHourlyTotalsOffsets
			{
				fieldOffsetReadFilteredHourlyTotalsPLUNumber = 1,
				fieldOffsetReadFilteredHourlyTotalsTransactionType,
				fieldOffsetReadFilteredHourlyTotalsStartDate,
				fieldOffsetReadFilteredHourlyTotalsEndDate,

				endReadFilteredHourlyTotalsOffsets
			} ReadFilteredHourlyTotalsOffsets_t;


			/* --- Constructors and destructor --- */
			ReadFilteredHourlyTotalsMessage(const char * data, int len, int formatType);
			~ReadFilteredHourlyTotalsMessage();


			/* --- Getters and setters --- */
			const std::string& getPLUNumber() const { return pluNumber_; }
			void setPLUNumber(const std::string& plu) { pluNumber_ = plu; }

			const std::string& getTransactionType() const { return transactionType_; }
			void setTransactionType(const std::string& type) { transactionType_ = type; }

			const std::string& getStartDateTime() const { return startDateTime_; }
			void setStartDateTime(const std::string& datetime) { startDateTime_ = datetime; }

			const std::string& getEndDateTime() const { return endDateTime_; }
			void setEndDateTime(const std::string& datetime) { endDateTime_ = datetime; }
			

			/* --- Instance function members --- */


			/* --- Class function members --- */


		private:
			/* --- Data members --- */
			std::string pluNumber_;
			std::string transactionType_;
			std::string startDateTime_;
			std::string endDateTime_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif

			/* --- Constructors --- */
			ReadFilteredHourlyTotalsMessage();


			/* --- Instance function members --- */
			void init();
	};


}



#endif /* READFILTEREDHOURLYTOTALSMESSAGE_H */
