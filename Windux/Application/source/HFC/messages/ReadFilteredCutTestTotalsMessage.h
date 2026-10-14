#ifndef READFILTEREDCUTTESTTOTALSMESSAGE_H
#define READFILTEREDCUTTESTTOTALSMESSAGE_H



#include "HfcObject.h"



namespace hfc
{


class ReadFilteredCutTestTotalsMessage : public HfcObject
{
	public:
		/* --- Types and subtypes --- */
		typedef enum ReadFilteredCutTestTotalsOffsets
		{
			fieldOffsetReadFilteredCutTestTotalsCutTestNumber = 1,
			fieldOffsetReadFilteredCutTestTotalsProductNumber,
			fieldOffsetReadFilteredCutTestTotalsTransactionType,
			fieldOffsetReadFilteredCutTestTotalsStartDate,
			fieldOffsetReadFilteredCutTestTotalsEndDate,

			endReadFilteredCutTestTotalsOffsets
		} ReadFilteredCutTestTotalsOffsets_t;


		/* --- Constructors and destructor --- */
		ReadFilteredCutTestTotalsMessage(const char * data, int len, int formatType);
		~ReadFilteredCutTestTotalsMessage();


		/* --- Getters and setters --- */
		const std::string& getCutTestNumber() const { return cutTestNumber_; }
		void setCutTestNumber(const std::string& plu) { cutTestNumber_ = plu; }

		const std::string& getProductNumber() const { return productNumber_; }
		void setProductNumber(const std::string& plu) { productNumber_ = plu; }

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
		std::string cutTestNumber_;
		std::string productNumber_;
		std::string transactionType_;
		std::string startDateTime_;
		std::string endDateTime_;


		/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif

		/* --- Constructors --- */
		ReadFilteredCutTestTotalsMessage();


		/* --- Instance function members --- */
		void init();
};


}



#endif /* READFILTEREDCUTTESTTOTALSMESSAGE_H */
