#ifndef TRANSACTION_H
#define TRANSACTION_H



#include <string>


#include "HfcObject.h"

#include "TransactionType.h"
#include "Product.h"
#include "OperatorChange.h"
class HobOperator;

#include "tags/Tags.h"

#include "HobDateTime.h"
#include "util/e_string/utilstring.h"

#include "CSmartPtr.h"



namespace hfc
{


	class Transaction : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				VoidInvalid = -1,

				VoidNotVoided,
				VoidVoided,
				VoidVoiding,

				VoidLast
			} VoidFlag;

			typedef enum
			{
				ReadClearInvalid = -1,

				ReadClearNotRead,
				ReadClearRead,
				ReadClearCleared,

				ReadClearLast
			} ReadClearFlag;

			typedef enum
			{
				fieldOffsetScaleId = 1,
				fieldOffsetTransactionId,
				fieldOffsetTransactionGroup,
				fieldOffsetTransactionCommittedDateTime,
				fieldOffsetTransactionCommittedDate,
				fieldOffsetTransactionCommittedTime,
				fieldOffsetTransactionCommittedDayWeek,
				fieldOffsetTransactionVoid,
				fieldOffsetTransactionType,
				fieldOffsetTransactionAffectsProduction,
				fieldOffsetTransactionReadAndClear,
				fieldOffsetTransactionPackagesNumber,
				fieldOffsetTransactionPrintedSecurityLabel,
				fieldOffsetTransactionCOOLFlag,
				fieldOffsetTransactionTotalValue,
				fieldOffsetTransactionOldTotalValue,
				fieldOffsetTransactionCutTestNumber,
				fieldOffsetTransactionPercentOfPrimalWeight,
				fieldOffsetTransactionPercentOfPrimalCost,
				fieldOffsetProductNumber,
				fieldOffsetProductReportingClass,
				fieldOffsetProductType,
				fieldOffsetProductionPlanningType,
				fieldOffsetProductDescription,
				fieldOffsetProductPriceModifier,
				fieldOffsetProductBarcodePrefix,
				fieldOffsetProductBarcodeNumber,
				fieldOffsetProductBarcodeDigit,
				fieldOffsetProductUnitPrice,
				fieldOffsetProductExceptionPrice,
				fieldOffsetProductFinalPrice,
				fieldOffsetProductTare,
				fieldOffsetProductProportionalTare,
				fieldOffsetProductFinalTare,
				fieldOffsetProductByCount,
				fieldOffsetProductFinalNetWeight,
				fieldOffsetProductFixedWeight,
				fieldOffsetProductShelfLifeDays,
				fieldOffsetProductShelfLifeHours,
				fieldOffsetProductShelfLifeMinutes,			// already existed - orig location
				fieldOffsetProductLifeDays,
				fieldOffsetProductLifeHours,
				fieldOffsetProductLifeMinutes,				// already existed - orig location
				fieldOffsetProductLabel1,
				fieldOffsetProductLabel2,
				fieldOffsetProductLabel3,
				fieldOffsetProductLabel4,
				fieldOffsetProductGraphicName1,
				fieldOffsetProductGraphicName2,
				fieldOffsetProductGraphicName3,
				fieldOffsetProductGraphicName4,
				fieldOffsetProductCoolTrackingText,
				fieldOffsetProductCoolText,
				fieldOffsetProductPLUFlags,
				fieldOffsetProductOldBarcodeDigit,
				fieldOffsetProductOldUnitPrice,
				fieldOffsetProductOldFinalPrice,
				fieldOffsetProductOldTare,
				fieldOffsetProductOldByCount,
				fieldOffsetProductOldFixedWeight,
				fieldOffsetProductOldShelfLifeDays,
				fieldOffsetProductOldShelfLifeHours,
				fieldOffsetProductOldShelfLifeMinutes,
				fieldOffsetProductOldProductLifeDays,
				fieldOffsetProductOldProductLifeHours,
				fieldOffsetProductOldProductLifeMinutes,
				fieldOffsetProductOldLabel1,
				fieldOffsetProductOldLabel2,
				fieldOffsetProductOldLabel3,
				fieldOffsetProductOldLabel4,
				fieldOffsetProductOldGraphicName1,
				fieldOffsetProductOldGraphicName2,
				fieldOffsetProductOldGraphicName3,
				fieldOffsetProductOldGraphicName4,
				fieldOffsetOperatorId,
				fieldOffsetOperatorRFID,
				fieldOffsetOperatorName,
				fieldOffsetOperatorChangeType,
				fieldOffsetTransactionTypeName,
				fieldOffsetOperatorAccessLevel,
				fieldOffsetProductTareB,
				fieldOffsetProductOldTareB,
				fieldOffsetTransactionModeChange,
				fieldOffsetTransactionDeptNum,
				fieldOffsetTransactionTypeTextForLabel,
				fieldOffsetSweetheartWeight,

				endTransactionFieldOffsets
			} FieldOffsets;

			static const int _invalidTransactionId_ = 0;


			/* --- Constructors and destructor --- */
			Transaction();
			Transaction(int transactionType, int scaleId = 0);
			Transaction(const char * data, int len, int formatType);
			Transaction(const Transaction& rhs);
			~Transaction();


			/* --- Getters and setters --- */
			HobRecordNum getScaleId() const { return scaleId_; }
			void setScaleId(HobRecordNum id) { scaleId_ = id; debugSetter(fieldOffsetScaleId, scaleId_); }
			void setCurrentScaleId() { scaleId_ = 0; debugSetter(fieldOffsetScaleId, scaleId_); }

			HobRecordNum getTransactionId() const { return transactionId_; }
			void setTransactionId(HobRecordNum id) { transactionId_ = id; debugSetter(fieldOffsetTransactionId, transactionId_); }

			int getTransactionGroupNumber() const { return groupNumber_; }
			void setTransactionGroupNumber(int group) { groupNumber_ = group; debugSetter(fieldOffsetTransactionGroup, groupNumber_); }

			HobRecordNum getTransactionTypeNumber() const { return transactionTypeNumber_; }
			void setTransactionTypeNumber(HobRecordNum number) { transactionTypeNumber_ = number; debugSetter(fieldOffsetTransactionType, transactionTypeNumber_); }

			const std::string& getTransactionTypeName() const { return transactionTypeName_; }
			void setTransactionTypeName(const std::string& name) { transactionTypeName_ = name; debugSetter(fieldOffsetTransactionTypeName, transactionTypeName_); }

			const std::string& getTransactionTypeTextForLabel() const { return transactionTypeTextForLabel_; }
			void setTransactionTypeTextForLabel(const std::string& text) { transactionTypeTextForLabel_ = text; debugSetter(fieldOffsetTransactionTypeTextForLabel, transactionTypeTextForLabel_); }


			bool isTransactionCommitted() const { return committed_; }
			void setTransactionCommitted(bool committed = true) { committed ? setCommittedDateTime() : clearTransactionCommitted(); }
			void clearTransactionCommitted() { committed_ = false; committedDateTime_.clear(); }

			HobDateTime getCommittedDateTime() const { return HobDateTime::createDateTimeFromDBFormat(committedDateTime_); }
			const std::string& getCommittedDateTimeAsString() const { return committedDateTime_; }
			void setCommittedDateTime(const std::string& dt) { committedDateTime_ = dt; committed_ = true; debugSetter(fieldOffsetTransactionCommittedDateTime, committedDateTime_); }
			void setCommittedDateTime(const HobDateTime& dt = HobDateTime()) { committedDateTime_ = dt.convertToDBFormat(); committed_ = true; debugSetter(fieldOffsetTransactionCommittedDateTime, committedDateTime_); }

			VoidFlag getVoid() const { return void_; }
			const std::string& getVoidAsString() const;
			void clearVoid() { setVoid(VoidNotVoided); }
			void setVoid(VoidFlag flag) { void_ = flag; debugSetter(fieldOffsetTransactionVoid, void_); }

			TransactionType::AffectsProductionFlag getAffectsProductionFlag() const { return affectsProduction_; }
			const std::string& getAffectsProductionFlagAsString() const;
			void setAffectsProductionFlag(TransactionType::AffectsProductionFlag flag) { affectsProduction_ = flag; debugSetter(fieldOffsetTransactionAffectsProduction, affectsProduction_); }

			ReadClearFlag getReadClearFlag() const { return readClear_; }
			const std::string& getReadClearFlagAsString() const;
			void clearReadClearFlag() { setReadClearFlag(ReadClearNotRead); }
			void setReadClearFlag(ReadClearFlag flag) { readClear_ = flag; debugSetter(fieldOffsetTransactionReadAndClear, readClear_); }

			int getNumberPackages() const { return numberPackages_; }
			void incrementNumberPackages() { ++numberPackages_; debugSetter(fieldOffsetTransactionPackagesNumber, numberPackages_); }
			void setNumberPackages(int number) { numberPackages_ = number; debugSetter(fieldOffsetTransactionPackagesNumber, numberPackages_); }

			bool isPrintedSecurityLabel() const { return printedSecurityLabel_; }
			void setPrintedSecurityLabel(bool printed) { printedSecurityLabel_ = printed; debugSetter(fieldOffsetTransactionPrintedSecurityLabel, printedSecurityLabel_); }

			bool isCOOLTransaction() const { return coolTransaction_; }
			void setCOOLTransaction(bool flag = true) { coolTransaction_ = flag; debugSetter(fieldOffsetTransactionCOOLFlag, coolTransaction_); }

			HobPrice getTotalValue() const { return totalValue_; }
			void setTotalValue(HobPrice total) { totalValue_ = total; debugSetter(fieldOffsetTransactionTotalValue, totalValue_); }

			HobPrice getOldTotalValue() const { return oldTotalValue_; }
			void setOldTotalValue(HobPrice total) { oldTotalValue_ = total; debugSetter(fieldOffsetTransactionOldTotalValue, oldTotalValue_); }

			int getNumberItems() const { return numberPackages_ * (productType_ != Product::PRODUCTTYPE_RANDOMWEIGHT ? byCount_ : 1); }


			HobRecordNum getDeptNum() const { return deptNum_; }
			void setDeptNum(HobRecordNum deptNum) { deptNum_ = deptNum; debugSetter(fieldOffsetTransactionDeptNum, deptNum_); }

			HobRecordNum getProductNumber() const { return productNumber_; }
			void setProductNumber(HobRecordNum num) { productNumber_ = num; debugSetter(fieldOffsetProductNumber, productNumber_); }

			HobRecordNum getReportingClass() const { return reportingClass_; }
			void setReportingClass(HobRecordNum num) { reportingClass_ = num; debugSetter(fieldOffsetProductReportingClass, reportingClass_); }

			Product::ProductType getProductType() const { return productType_; }
			void setProductType(Product::ProductType type) { productType_ = type; debugSetter(fieldOffsetProductType, productType_); }

			Product::ProductionPlanningType getProductionPlanningType() const { return productionPlanningType_; }
			void setProductionPlanningType(Product::ProductionPlanningType type) { productionPlanningType_ = type; debugSetter(fieldOffsetProductionPlanningType, productionPlanningType_); }

			const std::string& getProductDescription() const { return productDescription_; }
			void setProductDescription(const std::string& desc) { productDescription_.assign(desc); debugSetter(fieldOffsetProductDescription, productDescription_); }

			short getPriceModifier() const { return priceModifier_; }
			void setPriceModifier(short mod) { priceModifier_ = mod; debugSetter(fieldOffsetProductPriceModifier, priceModifier_); }

			int getBarcodePrefix() const { return barcodePrefix_; }
			void setBarcodePrefix(int prefix) { barcodePrefix_ = prefix; debugSetter(fieldOffsetProductBarcodePrefix, barcodePrefix_); }

			const std::string& getBarcodeNumber() const { return barcodeNumber_; }
			void setBarcodeNumber(const std::string& code) { barcodeNumber_.assign(code); debugSetter(fieldOffsetProductBarcodeNumber, barcodeNumber_); }

			HobPrice getUnitPrice() const { return unitPrice_; }
			void setUnitPrice(HobPrice price) { unitPrice_ = price; debugSetter(fieldOffsetProductUnitPrice, unitPrice_); }

			HobPrice getExceptionPrice() const { return exceptionPrice_; }
			void setExceptionPrice(HobPrice price) { exceptionPrice_ = price; debugSetter(fieldOffsetProductExceptionPrice, exceptionPrice_); }

			HobValue64 getFinalPrice() const { return finalPrice_; }
			void setFinalPrice(HobValue64 price) { finalPrice_ = price; debugSetter(fieldOffsetProductFinalPrice, finalPrice_); }

			HobWeight getTare() const { return tare_; }
			void setTare(HobWeight tare) { tare_ = tare; debugSetter(fieldOffsetProductTare, tare_); }

			int getProportionalTare() const { return proportionalTare_; }
			void setProportionalTare(int prop) { proportionalTare_ = prop; debugSetter(fieldOffsetProductProportionalTare, proportionalTare_); }

			HobWeight getFinalTare() const { return finalTare_; }
			void setFinalTare(HobWeight tare) { finalTare_ = tare; debugSetter(fieldOffsetProductFinalTare, finalTare_); }

			int getByCount() const { return byCount_; }
			void setByCount(int bc) { byCount_ = bc; debugSetter(fieldOffsetProductByCount, byCount_); }

			bool getByCountForced() const { return byCountForced_; }
			void setByCountForced(bool bcf) { byCountForced_ = bcf; }

			bool getByCountOldZero() const { return byCountOldZero_; }
			void setByCountOldZero(bool bcz) { byCountOldZero_ = bcz; }

			HobWeight getFinalNetWeight() const { return finalNetWeight_; }
			void setFinalNetWeight(HobWeight nw) { finalNetWeight_ = nw; debugSetter(fieldOffsetProductFinalNetWeight, finalNetWeight_); }

			HobWeight getFixedWeight() const { return fixedWeight_; }
			void setFixedWeight(HobWeight nw) { fixedWeight_ = nw; debugSetter(fieldOffsetProductFixedWeight, fixedWeight_); }

			int getShelfLifeDays() const { return shelfLifeDays_; }
			void setShelfLifeDays(int days) { shelfLifeDays_ = days; debugSetter(fieldOffsetProductShelfLifeDays, shelfLifeDays_); }

			short getShelfLifeHours() const { return shelfLifeHours_; }
			void setShelfLifeHours(short hours) { shelfLifeHours_ = hours; debugSetter(fieldOffsetProductShelfLifeHours, shelfLifeHours_); }

			short getShelfLifeMinutes() const { return shelfLifeMinutes_; }
			void setShelfLifeMinutes(short minutes) { shelfLifeMinutes_ = minutes; debugSetter(fieldOffsetProductShelfLifeMinutes, shelfLifeMinutes_); }

			int getProductLifeDays() const { return productLifeDays_; }
			void setProductLifeDays(int days) { productLifeDays_ = days; debugSetter(fieldOffsetProductLifeDays, productLifeDays_); }

			short getProductLifeHours() const { return productLifeHours_; }
			void setProductLifeHours(short hours) { productLifeHours_ = hours; debugSetter(fieldOffsetProductLifeHours, productLifeHours_); }

			short getProductLifeMinutes() const { return productLifeMinutes_; }
			void setProductLifeMinutes(short minutes) { productLifeMinutes_ = minutes; debugSetter(fieldOffsetProductLifeMinutes, productLifeMinutes_); }

			HobRecordNum getLabel1() const { return label1_; }
			void setLabel1(HobRecordNum label) { label1_ = label; debugSetter(fieldOffsetProductLabel1, label1_); }

			HobRecordNum getLabel2() const { return label2_; }
			void setLabel2(HobRecordNum label) { label2_ = label; debugSetter(fieldOffsetProductLabel2, label2_); }

			HobRecordNum getLabel3() const { return label3_; }
			void setLabel3(HobRecordNum label) { label3_ = label; debugSetter(fieldOffsetProductLabel3, label3_); }

			HobRecordNum getLabel4() const { return label4_; }
			void setLabel4(HobRecordNum label) { label4_ = label; debugSetter(fieldOffsetProductLabel4, label4_); }

			const std::string& getGraphicName1() const { return graphicName1_; }
			void setGraphicName1(const std::string& name) { graphicName1_.assign(name); debugSetter(fieldOffsetProductGraphicName1, graphicName1_); }

			const std::string& getGraphicName2() const { return graphicName2_; }
			void setGraphicName2(const std::string& name) { graphicName2_.assign(name); debugSetter(fieldOffsetProductGraphicName2, graphicName2_); }

			const std::string& getGraphicName3() const { return graphicName3_; }
			void setGraphicName3(const std::string& name) { graphicName3_.assign(name); debugSetter(fieldOffsetProductGraphicName3, graphicName3_); }

			const std::string& getGraphicName4() const { return graphicName4_; }
			void setGraphicName4(const std::string& name) { graphicName4_.assign(name); debugSetter(fieldOffsetProductGraphicName4, graphicName4_); }

			const std::string& getCOOLText() const { return coolText_; }
			void setCOOLText(const std::string& text) { coolText_.assign(text); debugSetter(fieldOffsetProductCoolText, coolText_); }

			const std::string& getCOOLTrackingText() const { return coolTrackingText_; }
			void setCOOLTrackingText(const std::string& text) { coolTrackingText_.assign(text); debugSetter(fieldOffsetProductCoolTrackingText, coolTrackingText_); }


			int getOldBarcodeDigit() const { return oldBarcodeDigit_; }
			void setOldBarcodeDigit(int digit) { oldBarcodeDigit_ = digit; debugSetter(fieldOffsetProductOldBarcodeDigit, oldBarcodeDigit_); }

			HobPrice getOldUnitPrice() const { return oldUnitPrice_; }
			void setOldUnitPrice(HobPrice price) { oldUnitPrice_ = price; debugSetter(fieldOffsetProductOldUnitPrice, oldUnitPrice_); }

			HobValue64 getOldFinalPrice() const { return oldFinalPrice_; }
			void setOldFinalPrice(HobValue64 price) { oldFinalPrice_ = price; debugSetter(fieldOffsetProductOldFinalPrice, oldFinalPrice_); }

			HobWeight getOldTare() const { return oldTare_; }
			void setOldTare(HobWeight tare) { oldTare_ = tare; debugSetter(fieldOffsetProductOldTare, oldTare_); }

			int getOldByCount() const { return oldByCount_; }
			void setOldByCount(int bc) { oldByCount_ = bc; debugSetter(fieldOffsetProductOldByCount, oldByCount_); }

			HobWeight getOldFixedWeight() const { return oldFixedWeight_; }
			void setOldFixedWeight(HobWeight nw) { oldFixedWeight_ = nw; debugSetter(fieldOffsetProductOldFixedWeight, oldFixedWeight_); }

			int getOldShelfLifeDays() const { return oldShelfLifeDays_; }
			void setOldShelfLifeDays(int days) { oldShelfLifeDays_ = days; debugSetter(fieldOffsetProductOldShelfLifeDays, oldShelfLifeDays_); }

			int getOldShelfLifeHours() const { return oldShelfLifeHours_; }
			void setOldShelfLifeHours(int hours) { oldShelfLifeHours_ = hours; debugSetter(fieldOffsetProductOldShelfLifeHours, oldShelfLifeHours_); }

			int getOldShelfLifeMinutes() const { return oldShelfLifeMinutes_; }
			void setOldShelfLifeMinutes(int minutes) { oldShelfLifeMinutes_ = minutes; debugSetter(fieldOffsetProductOldShelfLifeMinutes, oldShelfLifeMinutes_); }

			int getOldProductLifeDays() const { return oldProductLifeDays_; }
			void setOldProductLifeDays(int days) { oldProductLifeDays_ = days; debugSetter(fieldOffsetProductOldProductLifeDays, oldProductLifeDays_); }

			int getOldProductLifeHours() const { return oldProductLifeHours_; }
			void setOldProductLifeHours(int hours) { oldProductLifeHours_ = hours; debugSetter(fieldOffsetProductLifeHours, oldProductLifeHours_); }

			int getOldProductLifeMinutes() const { return oldProductLifeMinutes_; }
			void setOldProductLifeMinutes(int minutes) { oldProductLifeMinutes_ = minutes; debugSetter(fieldOffsetProductLifeMinutes, oldProductLifeMinutes_); }

			HobRecordNum getOldLabel1() const { return oldLabel1_; }
			void setOldLabel1(HobRecordNum label) { oldLabel1_ = label; debugSetter(fieldOffsetProductOldLabel1, oldLabel1_); }

			HobRecordNum getOldLabel2() const { return oldLabel2_; }
			void setOldLabel2(HobRecordNum label) { oldLabel2_ = label; debugSetter(fieldOffsetProductOldLabel2, oldLabel2_); }

			HobRecordNum getOldLabel3() const { return oldLabel3_; }
			void setOldLabel3(HobRecordNum label) { oldLabel3_ = label; debugSetter(fieldOffsetProductOldLabel3, oldLabel3_); }

			HobRecordNum getOldLabel4() const { return oldLabel4_; }
			void setOldLabel4(HobRecordNum label) { oldLabel4_ = label; debugSetter(fieldOffsetProductOldLabel4, oldLabel4_); }

			const std::string& getOldGraphicName1() const { return oldGraphicName1_; }
			void setOldGraphicName1(const std::string& name) { oldGraphicName1_.assign(name); debugSetter(fieldOffsetProductOldGraphicName1, oldGraphicName1_); }

			const std::string& getOldGraphicName2() const { return oldGraphicName2_; }
			void setOldGraphicName2(const std::string& name) { oldGraphicName2_.assign(name); debugSetter(fieldOffsetProductOldGraphicName2, oldGraphicName2_); }

			const std::string& getOldGraphicName3() const { return oldGraphicName3_; }
			void setOldGraphicName3(const std::string& name) { oldGraphicName3_.assign(name); debugSetter(fieldOffsetProductOldGraphicName3, oldGraphicName3_); }

			const std::string& getOldGraphicName4() const { return oldGraphicName4_; }
			void setOldGraphicName4(const std::string& name) { oldGraphicName4_.assign(name); debugSetter(fieldOffsetProductOldGraphicName4, oldGraphicName4_); }


			const std::string& getOperatorId() const { return operatorId_; }
			void setOperatorId(const std::string& id) { operatorId_.assign(id); debugSetter(fieldOffsetOperatorId, operatorId_); }

			const std::string& getOperatorRFID() const { return operatorRFID_; }
			void setOperatorRFID(const std::string& rfid) { operatorRFID_.assign(rfid); debugSetter(fieldOffsetOperatorRFID, operatorRFID_); }

			const std::string& getOperatorName() const { return operatorName_; }
			void setOperatorName(const std::string& name) { operatorName_.assign(name); debugSetter(fieldOffsetOperatorName, operatorName_); }

			const std::string& getOperatorAccessLevel() const { return operatorAccessLevel_; }
			void setOperatorAccessLevel(const std::string& level) { operatorAccessLevel_.assign(level); debugSetter(fieldOffsetOperatorAccessLevel, operatorAccessLevel_); }

			HobWeight getTareB() const { return tareB_; }
			void setTareB(HobWeight tareB) { tareB_ = tareB; debugSetter(fieldOffsetProductTareB, tareB_); }

			HobWeight getOldTareB() const { return oldTareB_; }
			void setOldTareB(HobWeight tareB) { oldTareB_ = tareB; debugSetter(fieldOffsetProductOldTareB, oldTareB_); }

			// Moved the methods for OperatorChangeType that were right here to the CPP file WHERE IT MAKES SENSE.

			unsigned int getOperatorChangeType() const;
			void setOperatorChangeType(unsigned int change);
			std::string getOperatorChangeTypeAsString() const;

			bool isOperatorChangeType(OperatorChange::OperatorChangeableField field) const;
			void addOperatorChangeType(OperatorChange::OperatorChangeableField field);
			void clearOperatorChangeType(OperatorChange::OperatorChangeableField field);

			HobErrorCode getErrorCode() const { return errorCode_; }
			bool isRecordValid() const { return errorCode_ == RECORD_OK; }
			void setErrorCode(HobErrorCode errorCode);

			//
			/* --- Instance function members --- */
			void updateTransactionTotalValues();
			void updateSupervisorChangesValues(bool origBC, int bc, bool fbc, HobWeight tareA, HobWeight tareB, HobWeight fw, HobPrice price, int modeChg);

			void associateTransactionType(Util::CSmartPtr<TransactionType> transType);
			void associateOperator(const HobOperator* oper);

			std::string getOperatorChangeOldValue(OperatorChange::OperatorChangeableField field, int format = BrowserFormat);
			std::string getOperatorChangeNewValue(OperatorChange::OperatorChangeableField field, int format = BrowserFormat);

			HobRecordNum getCutTestNumber() const { return cutTestNumber_; }
			void setCutTestNumber(HobRecordNum number) { cutTestNumber_ = number; debugSetter(fieldOffsetTransactionCutTestNumber, cutTestNumber_); }

			int getPercentOfPrimalWeight() const { return percentOfPrimalWeight_; }
			void setPercentOfPrimalWeight(int pct) { percentOfPrimalWeight_ = pct; debugSetter(fieldOffsetTransactionPercentOfPrimalWeight, percentOfPrimalWeight_); }

			int getPercentOfPrimalCost() const { return percentOfPrimalCost_; }
			void setPercentOfPrimalCost(int pct) { percentOfPrimalCost_ = pct; debugSetter(fieldOffsetTransactionPercentOfPrimalCost, percentOfPrimalCost_); }

			int getModeChange() const { return modeChange_; }
			void setModeChange(int mc) { modeChange_ = mc; debugSetter(fieldOffsetTransactionModeChange, modeChange_); }

			HobWeight getSweetheartWeight() const { return sweetheartWeight_; }
			void setSweetheartWeight(HobWeight w) { sweetheartWeight_ = w; debugSetter(fieldOffsetSweetheartWeight, sweetheartWeight_); }

			std::string getLabelOutputFilename() const { return labelOutputFilename; }
			void setLabelOutputFilename(const std::string filename) { labelOutputFilename = filename; }

			const char * serialize(int format) const;
			void show() const;


		protected:
			/* --- Instance function members --- */
			const std::string& serializeField(FieldOffsets offset, int format = BrowserFormat) const;


		private:
			/* --- Instance Data members --- */
			HobErrorCode errorCode_;

			HobRecordNum scaleId_;
			HobRecordNum transactionId_;
			int groupNumber_;
			HobRecordNum transactionTypeNumber_;
			std::string transactionTypeName_;			//	Added to print first letter of TransType/TotalType name on label (CCD).
			std::string transactionTypeTextForLabel_;
			mutable bool committed_;                    // No need to persist
			std::string committedDateTime_;
			VoidFlag void_;
			TransactionType::AffectsProductionFlag affectsProduction_;
			ReadClearFlag readClear_;
			int numberPackages_;
			bool printedSecurityLabel_;
			bool coolTransaction_;
			HobPrice totalValue_;
			HobPrice oldTotalValue_;

			HobRecordNum deptNum_;
			HobRecordNum productNumber_;
			HobRecordNum reportingClass_;
			Product::ProductType productType_;
			Product::ProductionPlanningType productionPlanningType_;
			std::string productDescription_;
			short priceModifier_;
			int barcodePrefix_;
			std::string barcodeNumber_;
			HobPrice unitPrice_;
			HobPrice exceptionPrice_;
			HobValue64 finalPrice_;
			HobWeight tare_;
			int proportionalTare_;
			HobWeight finalTare_;
			int byCount_;
			bool byCountForced_;
			bool byCountOldZero_;
			HobWeight finalNetWeight_;
			HobWeight fixedWeight_;        	//	For Fixed weight or Fluid ounce PLU
			int shelfLifeDays_;
			short shelfLifeHours_;
			short shelfLifeMinutes_;
			int productLifeDays_;
			short productLifeHours_;
			short productLifeMinutes_;

			HobRecordNum label1_;
			HobRecordNum label2_;
			HobRecordNum label3_;
			HobRecordNum label4_;

			std::string graphicName1_;
			std::string graphicName2_;
			std::string graphicName3_;
			std::string graphicName4_;
			std::string coolText_;
			std::string coolTrackingText_;

			int oldBarcodeDigit_;
			HobPrice oldUnitPrice_;
			HobValue64 oldFinalPrice_;
			HobWeight oldTare_;
			int oldByCount_;
			HobWeight oldFixedWeight_;
			int oldShelfLifeDays_;
			int oldShelfLifeHours_;
			int oldShelfLifeMinutes_;
			int oldProductLifeDays_;
			int oldProductLifeHours_;
			int oldProductLifeMinutes_;
			HobRecordNum oldLabel1_;
			HobRecordNum oldLabel2_;
			HobRecordNum oldLabel3_;
			HobRecordNum oldLabel4_;

			std::string oldGraphicName1_;
			std::string oldGraphicName2_;
			std::string oldGraphicName3_;
			std::string oldGraphicName4_;

			std::string operatorId_;
			std::string operatorRFID_;
			std::string operatorName_;
			std::string operatorAccessLevel_;
			
			HobWeight tareB_;
			HobWeight oldTareB_;

			unsigned int operatorChangeType_;

			mutable HobString serializedField_;          // used to serialize fields

			HobRecordNum cutTestNumber_;		//	Access
			int percentOfPrimalWeight_;
			int percentOfPrimalCost_;
            int modeChange_;

			HobWeight sweetheartWeight_;

			Product * myProduct;

			std::string labelOutputFilename;	// Used for printing labels to Zebra printer.


			/* --- Class Data members --- */
			static const std::string _VoidInvalidName_;
			static const std::string _VoidNotVoidedName_;
			static const std::string _VoidVoidedName_;
			static const std::string _VoidVoidingName_;

			static const std::string _ProductionInvalidName_;
			static const std::string _ProductionNegativelyName_;
			static const std::string _ProductionNoImpactName_;
			static const std::string _ProductionPositivelyName_;

			static const std::string _ReadClearInvalidName_;
			static const std::string _ReadClearNotReadName_;
			static const std::string _ReadClearReadName_;
			static const std::string _ReadClearClearedName_;

			static const std::string _OperatorChangeTypeNoChange_;
			static const std::string _OperatorChangeUnitPriceName_;
			static const std::string _OperatorChangeTareName_;
			static const std::string _OperatorChangeByCountName_;
			static const std::string _OperatorChangeShelfLifeName_;
			static const std::string _OperatorChangeProductLifeName_;
			static const std::string _OperatorChangeLabelType1Name_;
			static const std::string _OperatorChangeLabelType2Name_;
			static const std::string _OperatorChangeFixedWeightName_;
			static const std::string _OperatorChangeUPCDigitName_;
			static const std::string _OperatorChangeFinalPriceName_;
			static const std::string _OperatorChangeGraphic1Name_;
			static const std::string _OperatorChangeShelfLifeHoursName_;
			static const std::string _OperatorChangeShelfLifeMinutesName_;          // already present
			static const std::string _OperatorChangeProductLifeHoursName_;
			static const std::string _OperatorChangeProductLifeMinutesName_;        // already present
			static const std::string _OperatorChangeLabelType3Name_;
			static const std::string _OperatorChangeLabelType4Name_;
			static const std::string _OperatorChangeGraphic2Name_;
			static const std::string _OperatorChangeGraphic3Name_;
			static const std::string _OperatorChangeGraphic4Name_;
            static const std::string _OperatorChangeTareBName_;              

			static const std::string _scaleIdName_;
			static const std::string _transactionIdName_;
			static const std::string _transactionGroupName_;
			static const std::string _transactionTypeName_;
			static const std::string _transactionCommittedDateTimeName_;
			static const std::string _transactionVoidName_;
			static const std::string _transactionAffectsProductionName_;
			static const std::string _transactionReadAndClearName_;
			static const std::string _transactionPackagesNumberName_;
			static const std::string _transactionPrintedSecurityLabelName_;
			static const std::string _transactionCOOLFlagName_;
			static const std::string _transactionTotalValueName_;
			static const std::string _transactionOldTotalValueName_;
			static const std::string _transactionCutTestNumber_;
			static const std::string _transactionPercentOfPrimalWeight_;
			static const std::string _transactionPercentOfPrimalCost_;
            static const std::string _transactionModeChange_;
			static const std::string _deptNumName_;
			static const std::string _productNumberName_;
			static const std::string _productReportingClassName_;
			static const std::string _productTypeName_;
			static const std::string _productionPlanningTypeName_;
			static const std::string _productDescriptionName_;
			static const std::string _productPriceModifierName_;
			static const std::string _productBarcodePrefixName_;
			static const std::string _productBarcodeNumberName_;
			static const std::string _productUnitPriceName_;
			static const std::string _productExceptionPriceName_;
			static const std::string _productFinalPriceName_;
			static const std::string _productTareName_;
			static const std::string _productProportionalTareName_;
			static const std::string _productFinalTareName_;
			static const std::string _productByCountName_;
			static const std::string _productFinalNetWeightName_;
			static const std::string _productFixedWeightName_;
			static const std::string _productShelfLifeDaysName_;
			static const std::string _productShelfLifeHoursName_;
			static const std::string _productShelfLifeMinutesName_;
			static const std::string _productLifeDaysName_;
			static const std::string _productLifeHoursName_;
			static const std::string _productLifeMinutesName_;
			static const std::string _productLabel1Name_;
			static const std::string _productLabel2Name_;
			static const std::string _productLabel3Name_;
			static const std::string _productLabel4Name_;
			static const std::string _productGraphicName1Name_;
			static const std::string _productGraphicName2Name_;
			static const std::string _productGraphicName3Name_;
			static const std::string _productGraphicName4Name_;
			static const std::string _productTareBName_;
			static const std::string _productCoolTextName_;
			static const std::string _productCoolTrackingTextName_;
			static const std::string _productOldBarcodeDigitName_;
			static const std::string _productOldUnitPriceName_;
			static const std::string _productOldFinalPriceName_;
			static const std::string _productOldTareName_;
			static const std::string _productOldByCountName_;
			static const std::string _productOldFixedWeightName_;
			static const std::string _productOldShelfLifeDaysName_;
			static const std::string _productOldShelfLifeHoursName_;
			static const std::string _productOldShelfLifeMinutesName_;
			static const std::string _productOldProductLifeDaysName_;
			static const std::string _productOldProductLifeHoursName_;
			static const std::string _productOldProductLifeMinutesName_;
			static const std::string _productOldLabel1Name_;
			static const std::string _productOldLabel2Name_;
			static const std::string _productOldLabel3Name_;
			static const std::string _productOldLabel4Name_;
			static const std::string _productOldGraphicName1Name_;
			static const std::string _productOldGraphicName2Name_;
			static const std::string _productOldGraphicName3Name_;
			static const std::string _productOldGraphicName4Name_;
			static const std::string _productOldTareBName_;
			static const std::string _operatorIdName_;
			static const std::string _operatorNameName_;
			static const std::string _operatorChangeTypeName_;
            static const std::string _modeChangeName_;
			static const std::string _sweetheartWeightName_;
			static const std::string _unknownFieldOffsetName_;

			static FieldOffsetsList _dbIndependentFieldList_, _evolTaggedFieldList_, _readCompatibilityFieldList_;

#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif


			/* --- Instance function members --- */
			void init();
			FieldOffsets convertOperatorChangeableField(OperatorChange::OperatorChangeableField field, bool oldValue = false);

			void debugSetter(FieldOffsets offset, bool value) { debugSetter(offset, util_string::booltostring(value)); }
			void debugSetter(FieldOffsets offset, int value) { debugSetter(offset, util_string::inttostring(value)); }
			void debugSetter(FieldOffsets offset, unsigned int value) { debugSetter(offset, util_string::uinttostring(value)); }
			
            void debugSetter(FieldOffsets offset, long value) { debugSetter(offset, util_string::longtostring(value)); }
			void debugSetter(FieldOffsets offset, HobValue64 value) { debugSetter(offset, util_string::int64tostring((__int64)value)); }
			
			void debugSetter(FieldOffsets offset, const std::string& value)
					{ Debug::trace("Transaction.setting", "Setting " + getFieldOffsetAsString(offset) + " to '" + value + "'"); }


			/* --- Class function members --- */
			static void initFieldLists();
			static const std::string& getFieldOffsetAsString(FieldOffsets offset);
	};


}

#endif //TRANSACTION_H
