#ifndef TRANSACTIONTYPE_H
#define TRANSACTIONTYPE_H



#include "HfcObject.h"


#include "tags/Tags.h"



namespace hfc
{


	class TransactionType : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				AffectsProductionInvalid = -2,

				AffectsProductionNegatively,
				AffectsProductionNone,
				AffectsProductionPositively,

				AffectsProductionLast
			} AffectsProductionFlag;

			// flag bits for the total type flag
			typedef enum
			{
				TTUseBgColor		 = 0x0100,
				TTUnused_1		 	 = 0x0200,
				TTUnused_2		 	 = 0x0400,
				TTUnused_3		 	 = 0x0800,
				TTAllowBackDating	 = 0x1000,
				TTProductTotals	     = 0x2000,
				TTManualChangeDigit  = 0x4000,
				TTAutoChangeDigit	 = 0x8000
			} TransactionTypeFlags;

			static const HobRecordNum _PRODUCTION_TYPE_ = 1L;


			/* --- Constructors and destructor --- */
			TransactionType();
			TransactionType(const char * data, int len, int formatType);
			~TransactionType();


			/* --- Getters and setters --- */			
			HobErrorCode getErrorCode() const { return errorCode_; }
			void setErrorCode(HobErrorCode errorCode);
			bool isRecordValid() const { return errorCode_ == RECORD_OK; }


			HobRecordNum getNumber() const { return number_; }
			void setNumber(HobRecordNum number) { number_ = number; }

			const std::string& getName1() const { return name1_; }
			void setName1(const std::string& name) { name1_ = name; }

			const std::string& getName2() const { return name2_; }
			void setName2(const std::string& name) { name2_ = name; }

			HobWeight getGrossWeight() const { return grossWeight_; }
			void setGrossWeight(HobWeight weight) { grossWeight_ = weight; }

			AffectsProductionFlag getAffectsProduction() const { return number_ == _PRODUCTION_TYPE_ ? AffectsProductionPositively : affectsProduction_; }
			void setAffectsProduction(AffectsProductionFlag affects) { affectsProduction_ = (number_ == _PRODUCTION_TYPE_ ? AffectsProductionPositively : affects); }

			bool isAutomaticChangeUPCDigit() const { return automaticChangeUPCDigit_; }
			void setAutomaticChangeUPCDigit(bool automatic) { automaticChangeUPCDigit_ = automatic; }

			bool isManualChangeUPCDigit() const { return manualChangeUPCDigit_; }
			void setManualChangeUPCDigit(bool manual) { manualChangeUPCDigit_ = manual; }

			const std::string& getUPCChangeDigit() const { return upcChangeDigit_; }
			void setUPCChangeDigit(const std::string& digit) { upcChangeDigit_.assign(digit, 0, 1); }

			bool isAllowProductBackDating() const { return allowProductBackDating_; }
			void setAllowProductBackDating(bool allow) { allowProductBackDating_ = allow; }

			bool isUseBackgroundColor() const { return useBackgroundColor_; }
			void setUseBackgroundColor(bool allow) { useBackgroundColor_ = allow; }

			long getBackgroundColor() const { return backgroundColor_; }
			void setBackgroundColor(long c) { backgroundColor_ = c; }

			const std::string& getTextForLabel() const { return textForLabel_; }
			void setTextForLabel(const std::string& txt) { textForLabel_ = txt; }


			UINT16 getTransactionTypeFlag() const { refreshTransactionTypeFlag(); return transactionTypeFlag_; }
			void setTransactionTypeFlag(UINT16 flag) { transactionTypeFlag_ = flag; }


			/* --- Instance function members --- */
			void updateTransactionType(const TransactionType *newTransactionType);

			const char * serialize(int format) const;


			/* --- Class function members --- */
			static void initFieldList();
			

		private:
			/* --- Instance Data Members --- */
			HobErrorCode errorCode_;
			
			HobRecordNum number_;
			std::string name1_;
			std::string name2_;
			HobWeight grossWeight_;					//	Used for cutting test?
			AffectsProductionFlag affectsProduction_;
			bool automaticChangeUPCDigit_;
			bool manualChangeUPCDigit_;
			std::string upcChangeDigit_;
			bool allowProductBackDating_;
			bool useBackgroundColor_;
			long backgroundColor_;
			std::string textForLabel_;

			mutable UINT16 transactionTypeFlag_;


			/* --- Instance function members --- */
			void init();
			void refreshTransactionTypeFlag() const;
			void updateWithTransactionTypeFlag();

			std::string getFieldAsString(int fieldCode) const;
			bool isFieldQuoted(int fieldOffset) const;
			void updateField(int fieldCode, const HobString &token);


			/* --- Class data members --- */
			static FieldOffsetsList _fieldList_, _runModeList_;

#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


}



#endif //TRANSACTIONTYPE_H
