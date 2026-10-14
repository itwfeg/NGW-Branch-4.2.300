#ifndef READOPERATORCHANGESUMMARYMESSAGE_H
#define READOPERATORCHANGESUMMARYMESSAGE_H



#include "HfcObject.h"



namespace hfc
{


	class ReadOperatorChangeSummaryMessage : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				fieldOffsetPLUNumber = 1,
				fieldOffsetTransactionType,
				fieldOffsetStartDate,
				fieldOffsetEndDate,
				fieldOffsetViewChangesWhere,   // Operate Mode change is 0 or blank (legacy) and Supervisor Mode change is 1

				endOffsets
			} FieldOffsets;


			/* --- Constructors and destructor --- */
			ReadOperatorChangeSummaryMessage(const char * data, int len, int formatType);
			~ReadOperatorChangeSummaryMessage();


			/* --- Getters and setters --- */
			const std::string& getPLUNumber() const { return pluNumber_; }
			void setPLUNumber(const std::string& plu) { pluNumber_ = plu; }

			const std::string& getTransactionType() const { return transactionType_; }
			void setTransactionType(const std::string& type) { transactionType_ = type; }

			const std::string& getStartDateTime() const { return startDateTime_; }
			void setStartDateTime(const std::string& datetime) { startDateTime_ = datetime; }

			const std::string& getEndDateTime() const { return endDateTime_; }
			void setEndDateTime(const std::string& datetime) { endDateTime_ = datetime; }
			
			const std::string& getViewChangesWhere() const { return viewChangesWhere_; }
			void setViewChangesWhere(const std::string& whereAt) { viewChangesWhere_ = whereAt; }

			/* --- Instance function members --- */


			/* --- Class function members --- */


		private:
			/* --- Data members --- */
			std::string pluNumber_;
			std::string transactionType_;
			std::string startDateTime_;
			std::string endDateTime_;
			std::string viewChangesWhere_;   // Operate Mode change is 0 or blank (legacy) and Supervisor Mode change is 1


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif

			/* --- Constructors --- */
			ReadOperatorChangeSummaryMessage();


			/* --- Instance function members --- */
			void init();
	};


}



#endif /* READOPERATORCHANGESUMMARYMESSAGE_H */
