#ifndef READFILTEREDPRODUCTTOTALSMESSAGE_H
#define READFILTEREDPRODUCTTOTALSMESSAGE_H



#include "HfcObject.h"



namespace hfc
{


	class ReadFilteredProductTotalsMessage : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum ReadFilteredProductTotalsOffsets
			{
				fieldOffsetReadFilteredProductTotalsPLUNumber = 1,
				fieldOffsetReadFilteredProductTotalsTransactionType,
				fieldOffsetReadFilteredProductTotalsStartDate,
				fieldOffsetReadFilteredProductTotalsEndDate,

				endReadFilteredProductTotalsOffsets
			} ReadFilteredProductTotalsOffsets_t;


			/* --- Constructors and destructor --- */
			ReadFilteredProductTotalsMessage(const char * data, int len, int formatType);
			~ReadFilteredProductTotalsMessage();


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
			ReadFilteredProductTotalsMessage();


			/* --- Instance function members --- */
			void init();
	};


}



#endif /* READFILTEREDPRODUCTTOTALSMESSAGE_H */
