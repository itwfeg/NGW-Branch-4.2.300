#ifndef WRITETRANSACTIONWRITEMESSAGE_H
#define WRITETRANSACTIONWRITEMESSAGE_H



#include "HfcObject.h"


#include "HobList.h"
#include "CSmartPtr.h"


namespace hfc
{


	class Transaction;


	class WriteTransactionGroupMessage : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum WriteTransactionGroupOffsets
			{
				fieldOffsetWriteTransactionGroupIds = 1,
				fieldOffsetWriteTransactionGroupDateTimes,
				fieldOffsetWriteTransactionGroupOperators,
				fieldOffsetWriteTransactionGroupTypes,
				fieldOffsetWriteTransactionGroupPLUs,
				fieldOffsetWriteTransactionGroupWeights,
				fieldOffsetWriteTransactionGroupPrices,
				fieldOffsetWriteTransactionGroupItems,
				fieldOffsetWriteTransactionGroupPrevious,
				fieldOffsetWriteTransactionGroupNext,

				endWriteTransactionGroupOffsets
			} WriteTransactionGroupOffsets_t;


			/* --- Constructors and destructor --- */
			WriteTransactionGroupMessage(bool normalOrder = true);
			//WriteTransactionGroupMessage(const char * data, int len, int formatType);
			~WriteTransactionGroupMessage();


			/* --- Getters and setters --- */
			bool isReverseOrder() const { return reverseOrder_; }
			void setReverseOrder(bool reverse = true) { reverseOrder_ = reverse; }

			bool isPreviousGroupAvailable() const { return prevAvailable_; }
			void setPreviousGroupAvailable(bool available = true) { prevAvailable_ = available; }

			bool isNextGroupAvailable() const { return nextAvailable_; }
			void setNextGroupAvailable(bool available = true) { nextAvailable_ = available; }


			/* --- Instance function members --- */
			void addTransaction(Util::CSmartPtr<Transaction> trans);

			const char * serialize(int format) const;


			/* --- Class function members --- */


		private:
			/* --- Data members --- */
			bool reverseOrder_;
			bool prevAvailable_;
			bool nextAvailable_;
			HobList ids_;
			HobList times_;
			HobList operators_;
			HobList types_;
			HobList plus_;
			HobList weights_;
			HobList prices_;
			HobList items_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif

			/* --- Constructors --- */


			/* --- Instance function members --- */
	};


}



#endif /* WRITETRANSACTIONWRITEMESSAGE_H */
