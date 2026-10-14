#ifndef READOPERATORCHANGEGROUPMESSAGE_H
#define READOPERATORCHANGEGROUPMESSAGE_H



#include "HfcObject.h"



namespace hfc
{


	class ReadOperatorChangeGroupMessage : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				fieldOffsetPLUNumber = 1,
				fieldOffsetOperator,
				fieldOffsetTransactionType,
				fieldOffsetStartDate,
				fieldOffsetEndDate,
				fieldOffsetReadingGroup,
				fieldOffsetMaxQuantity,
				fieldOffsetViewChangesWhere,				// Operator change is blank (legacy) or 0, and Supervisor change is 1

				endFieldOffsets
			} FieldOffsets;

			typedef enum
			{
				GroupPrevious = -1,
				GroupFirst,
				GroupNext,

				GroupLast
			} ReadingGroup;


			/* --- Constructors and destructor --- */
			ReadOperatorChangeGroupMessage(const char * data, int len, int formatType);
			~ReadOperatorChangeGroupMessage();


			/* --- Getters and setters --- */
			const std::string& getPLUNumber() const { return pluNumber_; }
			void setPLUNumber(const std::string& plu) { pluNumber_.assign(plu); }

			const std::string& getOperatorId() const { return operatorId_; }
			void setOperatorId(const std::string& id) { operatorId_.assign(id); }

			const std::string& getTransactionType() const { return transactionType_; }
			void setTransactionType(const std::string& type) { transactionType_.assign(type); }

			const std::string& getStartDateTime() const { return startDateTime_; }
			void setStartDateTime(const std::string& datetime) { startDateTime_.assign(datetime); }

			const std::string& getEndDateTime() const { return endDateTime_; }
			void setEndDateTime(const std::string& datetime) { endDateTime_.assign(datetime); }
			
			ReadingGroup getReadingGroup() const { return group_; }
			void setReadingGroup(ReadingGroup group) { group_ = group; }

			int getMaxQuantity() const { return maxQuantity_; }
			void setMaxQuantity(int qty) { maxQuantity_ = qty; }

			const std::string& getViewChangesWhere() const { return viewChangesWhere_; }
			void setViewChangesWhere(const std::string& whereAt) { viewChangesWhere_.assign(whereAt); }

			/* --- Instance function members --- */
//			const char * serialize(int format) const;


			/* --- Class function members --- */


		private:
			/* --- Data members --- */
			std::string pluNumber_;
			std::string operatorId_;
			std::string transactionType_;
			std::string startDateTime_;
			std::string endDateTime_;
			ReadingGroup group_;
			int maxQuantity_;                   // Maximum number of records to read
			std::string viewChangesWhere_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif

			/* --- Constructors --- */
			ReadOperatorChangeGroupMessage();


			/* --- Instance function members --- */
			void init();
	};


}



#endif /* READOPERATORCHANGEGROUPMESSAGE_H */
