#ifndef OPERATORCHANGE_H
#define OPERATORCHANGE_H



#include "HfcObject.h"


#include "tags/Tags.h"

class HobOperator;

#include "HobDateTime.h"
#include "CSmartPtr.h"



namespace hfc
{


	class Transaction;


	class OperatorChange : public ::HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				OC_Unknown,			 //		0000 0000 0000 0000 0000 =      0

				OC_UnitPrice,		 //		0000 0000 0000 0000 0001 =      1
				OC_Tare,             //     0000 0000 0000 0000 0010 =      2
				OC_ByCount,          //     0000 0000 0000 0000 0100 =      4
				OC_ShelfLifeDays,    //     0000 0000 0000 0000 1000 =      8
				OC_ProductLifeDays,  // 5   0000 0000 0000 0001 0000 =     16
				OC_PrimaryLabel,	 //	    0000 0000 0000 0010 0000 =     32    Don't use for Evolution
				OC_SecondLabel,		 //	    0000 0000 0000 0100 0000 =     64    Don't use for Evolution
				OC_FixedWeight,      //     0000 0000 0000 1000 0000 =    128
				OC_UpcDigit,         //     0000 0000 0001 0000 0000 =    256
				OC_TotalPrice,       // 10  0000 0000 0010 0000 0000 =    512
				OC_Graphic1,		 //	    0000 0000 0100 0000 0000 =   1024    Don't use for Evolution
				OC_Graphic2,		 //	    0000 0000 1000 0000 0000 =   2048    Don't use for Evolution
				OC_Graphic3,		 //	    0000 0001 0000 0000 0000 =   4096    Don't use for Evolution
				OC_Graphic4,		 //	    0000 0010 0000 0000 0000 =   8192    Don't use for Evolution
				OC_ThirdLabel,		 // 15  0000 0100 0000 0000 0000 =  16384    Don't us for Evolution
				OC_ShelfLifeHours,   //     0000 1000 0000 0000 0000 =  32768
				OC_ProductLifeHours, //     0001 0000 0000 0000 0000 =  65536
				OC_TareB,            //     0010 0000 0000 0000 0000 = 131072
				
				// SOF-4904 Add shelfLifeMinutes to product and transactions - not supporting operator changes at this time, yet
				// OC_ShelfLifeMinutes,   //   will be big numbers  
				// OC_ProductLifeMinutes, //   will be big numbers - the last 2 sports available for this int

				OC_DoNotRecord,			//	Keep this one next to the end. Add new field changes before this.
				
				end_OpChangeFieldType
			} OperatorChangeableField;

			typedef enum
			{
				MC_OperateModeChange,
				MC_SupervisorModeChange,

				end_ModeChangeFieldType
			} ModeChangeableField;

			typedef enum
			{
				fieldOffsetOperatorId,
				fieldOffsetOperatorRFID,
				fieldOffsetDate,
				fieldOffsetTime,
				fieldOffsetDateTime,
				fieldOffsetTransactionType,
				fieldOffsetProductNumber,
				fieldOffsetTotalPrice,
				fieldOffsetTotalWeight,
				fieldOffsetChangedField,
				fieldOffsetNewValue,
				fieldOffsetOldValue,

				endFieldOffsets
			} FieldOffsets;

			/* --- Constructors and destructor --- */
			OperatorChange(Util::CSmartPtr<Transaction> trans, OperatorChange::OperatorChangeableField field);
//			OperatorChange(HobOperator * currentOperator, HobRecordNum productNum, int productFieldCode,
//					const std::string& newValue, const std::string& oldValue);
			OperatorChange(HobRecordNum productNum, int productFieldCode,
					long origValue, long newValue, HobOperator * currentOperator);
			~OperatorChange();


			/* --- Getter and setter methods --- */
			const std::string& getOperatorId() const { return operatorId_; }
			void setOperatorId(const std::string& id) { operatorId_.assign(id); }

			const std::string& getOperatorRFID() const { return operatorRFID_; }
			void setOperatorRFID(const std::string& id) { operatorRFID_.assign(id); }

			HobRecordNum getProductNumber() const { return productNumber_; }
			void setProductNumber(HobRecordNum num) { productNumber_ = num; }

			OperatorChangeableField getChangedField() const { return changedField_; }
			void setChangedField(OperatorChangeableField field) { changedField_ = field; }

			const std::string& getNewValue() const { return newValue_; }
			void setNewValue(const std::string& value) { newValue_.assign(value); }

			const std::string& getOldValue() const { return oldValue_; }
			void setOldValue(const std::string& value) { oldValue_.assign(value); }

			HobRecordNum getTransactionType() const { return transType_; }
			void setTransactionType(HobRecordNum num) { transType_ = num; }

			HobDateTime getChangeDateTime() const { return HobDateTime::createDateTimeFromDBFormat(dateTime_); }
			const std::string& getChangeDateTimeAsString() const { return dateTime_; }
			void setDateTime(const std::string& dt) { dateTime_.assign(dt); }

			HobWeight getTotalWeight() const { return totalWeight_; }
			void setTotalWeight(HobWeight weight) { totalWeight_ = weight; }

			HobPrice getTotalPrice() const { return totalPrice_; }
			void setTotalPrice(HobPrice price) { totalPrice_ = price; }


			HobErrorCode getErrorCode() const { return errorCode_; }
			bool isRecordValid() const { return errorCode_ == RECORD_OK; }
			void setErrorCode(HobErrorCode errorCode) { errorCode_ = errorCode; }


			/* --- Instance function members --- */
			void associateOperator(HobOperator* oper);
			void getFieldAsString(int fieldCode, HobString &result, int format) const;
			const char *serialize(int format) const;


		private:
			/* --- Instance data members --- */
			std::string operatorId_;
			std::string operatorRFID_;
			HobRecordNum productNumber_;
			OperatorChangeableField changedField_;
			std::string newValue_;
			std::string oldValue_;
			HobRecordNum transType_;
			std::string dateTime_;
			HobWeight totalWeight_;
			HobPrice totalPrice_;

			HobErrorCode errorCode_;


			/* --- Instance function members --- */
			void init();


			/* --- Class data members --- */
			static FieldOffsetsList _dbIndependentFieldList_, _evolTaggedFieldList_;

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif


			/* --- Class function members -- */
			static void initFieldList();
	};


}



#endif //OPERATORCHANGE_H
