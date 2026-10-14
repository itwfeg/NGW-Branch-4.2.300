#ifndef WRITEOPERATORCHANGEGROUPMESSAGE_H
#define WRITEOPERATORCHANGEGROUPMESSAGE_H



#include "HfcObject.h"
#include "OperatorChange.h"


#include "HobList.h"
#include "CSmartPtr.h"



namespace hfc
{


	class Transaction;


	class WriteOperatorChangeGroupMessage : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum Offsets
			{
				fieldOffsetOperatorName = 1,
				fieldOffsetTransactionIds,
				fieldOffsetDateTimes,
				fieldOffsetTransactionTypes,
				fieldOffsetPLUs,
				fieldOffsetTotalPrices,
				fieldOffsetChangedFields,
				fieldOffsetOriginalValues,
				fieldOffsetNewValues,
				fieldOffsetPrevious,
				fieldOffsetNext,

				endFieldOffsets
			} Offsets_t;


			/* --- Constructors and destructor --- */
			WriteOperatorChangeGroupMessage(const std::string& operatorName, bool normalOrder = true);
			~WriteOperatorChangeGroupMessage();


			/* --- Getters and setters --- */
			bool isPreviousGroupAvailable() const { return prevAvailable_; }
			void setPreviousGroupAvailable(bool available = true) { prevAvailable_ = available; }

			bool isNextGroupAvailable() const { return nextAvailable_; }
			void setNextGroupAvailable(bool available = true) { nextAvailable_ = available; }

			bool isReverseOrder() const { return reverseOrder_; }
			void setReverseOrder(bool reverse = true) { reverseOrder_ = reverse; }


			/* --- Instance function members --- */
			void addOperatorChange(Util::CSmartPtr<Transaction> trans, OperatorChange::OperatorChangeableField field);

			const char * serialize(int format) const;


			/* --- Class function members --- */


		private:
			/* --- Data members --- */
			std::string operatorName_;
			HobList transactionIds_;
			HobList dateTimes_;
			HobList transactionTypes_;
			HobList plus_;
			HobList totalPrices_;
			HobList changedFields_;
			HobList originalValues_;
			HobList newValues_;
			bool prevAvailable_;
			bool nextAvailable_;
			bool reverseOrder_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif

			/* --- Constructors --- */


			/* --- Instance function members --- */
	};


}



#endif /* WRITEOPERATORCHANGEGROUPMESSAGE_H */
