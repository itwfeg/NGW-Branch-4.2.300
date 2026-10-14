#ifndef READTRANSACTIONGROUPMESSAGE_H
#define READTRANSACTIONGROUPMESSAGE_H



#include "HfcObject.h"



namespace hfc
{


	class ReadTransactionGroupMessage : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum ReadTransactionGroupOffsets
			{
				fieldOffsetReadTransactionGroupPLUNumber = 1,
				fieldOffsetReadTransactionGroupOperator,
				fieldOffsetReadTransactionGroupStartDate,
				fieldOffsetReadTransactionGroupEndDate,
				fieldOffsetReadTransactionGroupReadingGroup,
				fieldOffsetReadTransactionGroupMaxQuantity,
				fieldOffsetReadTransactionViewChangesWhere,

				endReadTransactionGroupOffsets
			} ReadTransactionGroupOffsets_t;

			typedef enum
			{
				GroupPrevious = -1,
				GroupFirst,
				GroupNext,

				GroupLast
			} ReadingGroup;


			/* --- Constructors and destructor --- */
			ReadTransactionGroupMessage(const char * data, int len, int formatType);
			~ReadTransactionGroupMessage();


			/* --- Getters and setters --- */
			const std::string& getPLUNumber() const { return pluNumber_; }
			void setPLUNumber(const std::string& plu) { pluNumber_ = plu; }

			const std::string& getOperatorID() const { return operatorId_; }
			void setOperatorID(const std::string& id) { operatorId_ = id; }

			const std::string& getStartDateTime() const { return startDateTime_; }
			void setStartDateTime(const std::string& datetime) { startDateTime_ = datetime; }

			const std::string& getEndDateTime() const { return endDateTime_; }
			void setEndDateTime(const std::string& datetime) { endDateTime_ = datetime; }
			
			ReadingGroup getReadingGroup() const { return group_; }
			void setReadingGroup(ReadingGroup group) { group_ = group; }

			int getMaxQuantity() const { return maxQuantity_; }
			void setMaxQuantity(int qty) { maxQuantity_ = qty; }

			const std::string& getViewChangesWhere() const { return viewChangesWhere_; }
			void setViewChangesWhere(const std::string& whereAt) { viewChangesWhere_ = whereAt; }

			/* --- Instance function members --- */
			const char * serialize(int format) const;


			/* --- Class function members --- */


		private:
			/* --- Data members --- */
			std::string pluNumber_;
			std::string operatorId_;
			std::string startDateTime_;
			std::string endDateTime_;
			ReadingGroup group_;
			int maxQuantity_;                   // Maximum number of records to read
			std::string viewChangesWhere_;	    // Operate Mode change is 0 or blank (legacy) and Supervisor Mode change is 1


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif

			/* --- Constructors --- */
			ReadTransactionGroupMessage();


			/* --- Instance function members --- */
			void init();
	};


}



#endif /* READTRANSACTIONGROUPMESSAGE_H */
