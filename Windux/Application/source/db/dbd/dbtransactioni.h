#ifndef DBTRANSACTIONI_H
#define DBTRANSACTIONI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolListI;


	class DBTransactionI
	{
		public:

			virtual ~DBTransactionI() {}


			virtual DBTransactionI* clone() const = 0;
			virtual const DBTransactionI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getScaleID() const = 0;
			virtual void setScaleID(const std::string& scaleId) = 0;
			virtual bool isScaleIDNull() const { return false; }
			virtual bool isScaleIDWritable() const { return true; }

			virtual std::string getTransactionID() const = 0;
			virtual void setTransactionID(const std::string& id) = 0;
			virtual bool isTransactionIDNull() const { return false; }
			virtual bool isTransactionIDWritable() const { return true; }

			virtual std::string getTransactionGroup() const = 0;
			virtual void setTransactionGroup(const std::string& group) = 0;
			virtual bool isTransactionGroupNull() const { return false; }
			virtual void setTransactionGroupNull() = 0;
			virtual bool isTransactionGroupWritable() const { return true; }

			virtual std::string getTransactionDateTime() const = 0;
			virtual void setTransactionDateTime(const std::string& tdt) = 0;
			virtual bool isTransactionDateTimeNull() const { return false; }
			virtual void setTransactionDateTimeNull() = 0;
			virtual bool isTransactionDateTimeWritable() const { return true; }

			virtual int getTransactionType() const = 0;
			virtual void setTransactionType(int type) = 0;
			virtual bool isTransactionTypeNull() const { return false; }
			virtual void setTransactionTypeNull() = 0;
			virtual bool isTransactionTypeWritable() const { return true; }

			virtual int getTransactionVoid() const = 0;
			virtual void setTransactionVoid(int flag) = 0;
			virtual bool isTransactionVoidNull() const { return false; }
			virtual void setTransactionVoidNull() = 0;
			virtual bool isTransactionVoidWritable() const { return true; }

			virtual int getTransactionAffectsProduction() const = 0;
			virtual void setTransactionAffectsProduction(int flag) = 0;
			virtual bool isTransactionAffectsProductionNull() const { return false; }
			virtual void setTransactionAffectsProductionNull() = 0;
			virtual bool isTransactionAffectsProductionWritable() const { return true; }

			virtual int getTransactionReadCleared() const = 0;
			virtual void setTransactionReadCleared(int flag) = 0;
			virtual bool isTransactionReadClearedNull() const { return false; }
			virtual void setTransactionReadClearedNull() = 0;
			virtual bool isTransactionReadClearedWritable() const { return true; }

			virtual int getTransactionPackages() const = 0;
			virtual void setTransactionPackages(int packs) = 0;
			virtual bool isTransactionPackagesNull() const { return false; }
			virtual void setTransactionPackagesNull() = 0;
			virtual bool isTransactionPackagesWritable() const { return true; }

			virtual bool getTransactionUsedSecurityLabel() const = 0;
			virtual void setTransactionUsedSecurityLabel(bool value) = 0;
			virtual bool isTransactionUsedSecurityLabelNull() const { return false; }
			virtual void setTransactionUsedSecurityLabelNull() = 0;
			virtual bool isTransactionUsedSecurityLabelWritable() const { return true; }

			virtual bool getTransactionCOOLFlag() const = 0;
			virtual void setTransactionCOOLFlag(bool value) = 0;
			virtual bool isTransactionCOOLFlagNull() const { return false; }
			virtual void setTransactionCOOLFlagNull() = 0;
			virtual bool isTransactionCOOLFlagWritable() const { return true; }

			virtual int getTransactionTotalValue() const = 0;
			virtual void setTransactionTotalValue(int value) = 0;
			virtual bool isTransactionTotalValueNull() const { return false; }
			virtual void setTransactionTotalValueNull() = 0;
			virtual bool isTransactionTotalValueWritable() const { return true; }

			virtual int getTransactionOldTotalValue() const = 0;
			virtual void setTransactionOldTotalValue(int value) = 0;
			virtual bool isTransactionOldTotalValueNull() const { return false; }
			virtual void setTransactionOldTotalValueNull() = 0;
			virtual bool isTransactionOldTotalValueWritable() const { return true; }


			virtual std::string getDeptNumber() const = 0;
			virtual void setDeptNumber(const std::string& number) = 0;
			virtual bool isDeptNumberNull() const { return false; }
			virtual void setDeptNumberNull() = 0;
			virtual bool isDeptNumberWritable() const { return true; }

			virtual std::string getProductNumber() const = 0;
			virtual void setProductNumber(const std::string& plu) = 0;
			virtual bool isProductNumberNull() const { return false; }
			virtual void setProductNumberNull() = 0;
			virtual bool isProductNumberWritable() const { return true; }

			virtual std::string getReportingClass() const = 0;
			virtual void setReportingClass(const std::string& rc) = 0;
			virtual bool isReportingClassNull() const { return false; }
			virtual void setReportingClassNull() = 0;
			virtual bool isReportingClassWritable() const { return true; }

			virtual int getProductType() const = 0;
			virtual void setProductType(int type) = 0;
			virtual bool isProductTypeNull() const { return false; }
			virtual void setProductTypeNull() = 0;
			virtual bool isProductTypeWritable() const { return true; }

			virtual int getProductionPlanningType() const = 0;
			virtual void setProductionPlanningType(int type) = 0;
			virtual bool isProductionPlanningTypeNull() const { return false; }
			virtual void setProductionPlanningTypeNull() = 0;
			virtual bool isProductionPlanningTypeWritable() const { return true; }

			virtual std::string getProductDescription() const = 0;
			virtual void setProductDescription(const std::string& desc) = 0;
			virtual bool isProductDescriptionNull() const { return false; }
			virtual void setProductDescriptionNull() = 0;
			virtual bool isProductDescriptionWritable() const { return true; }

			virtual int getPriceFactor() const = 0;
			virtual void setPriceFactor(int pf) = 0;
			virtual bool isPriceFactorNull() const { return false; }
			virtual void setPriceFactorNull() = 0;
			virtual bool isPriceFactorWritable() const { return true; }

			virtual int getBarCodePrefix() const = 0;
			virtual void setBarCodePrefix(int bcpr) = 0;
			virtual bool isBarCodePrefixNull() const { return false; }
			virtual void setBarCodePrefixNull() = 0;
			virtual bool isBarCodePrefixWritable() const { return true; }

			virtual std::string getBarCodeNumber() const = 0;
			virtual void setBarCodeNumber(const std::string& barcode) = 0;
			virtual bool isBarCodeNumberNull() const { return false; }
			virtual void setBarCodeNumberNull() = 0;
			virtual bool isBarCodeNumberWritable() const { return true; }

			virtual int getUnitPrice() const = 0;
			virtual void setUnitPrice(int price) = 0;
			virtual bool isUnitPriceNull() const { return false; }
			virtual void setUnitPriceNull() = 0;
			virtual bool isUnitPriceWritable() const { return true; }

			virtual int getExceptionPrice() const = 0;
			virtual void setExceptionPrice(int xp) = 0;
			virtual bool isExceptionPriceNull() const { return false; }
			virtual void setExceptionPriceNull() = 0;
			virtual bool isExceptionPriceWritable() const { return true; }

			virtual int getFinalPrice() const = 0;
			virtual void setFinalPrice(int price) = 0;
			virtual bool isFinalPriceNull() const { return false; }
			virtual void setFinalPriceNull() = 0;
			virtual bool isFinalPriceWritable() const { return true; }

			virtual int getTare() const = 0;
			virtual void setTare(int t) = 0;
			virtual bool isTareNull() const { return false; }
			virtual void setTareNull() = 0;
			virtual bool isTareWritable() const { return true; }

			virtual int getProportionalTare() const = 0;
			virtual void setProportionalTare(int pt) = 0;
			virtual bool isProportionalTareNull() const { return false; }
			virtual void setProportionalTareNull() = 0;
			virtual bool isProportionalTareWritable() const { return true; }

			virtual int getFinalTare() const = 0;
			virtual void setFinalTare(int t) = 0;
			virtual bool isFinalTareNull() const { return false; }
			virtual void setFinalTareNull() = 0;
			virtual bool isFinalTareWritable() const { return true; }

			virtual int getByCount() const = 0;
			virtual void setByCount(int bc) = 0;
			virtual bool isByCountNull() const { return false; }
			virtual void setByCountNull() = 0;
			virtual bool isByCountWritable() const { return true; }

			virtual int getFinalNetWeight() const = 0;
			virtual void setFinalNetWeight(int nw) = 0;
			virtual bool isFinalNetWeightNull() const { return false; }
			virtual void setFinalNetWeightNull() = 0;
			virtual bool isFinalNetWeightWritable() const { return true; }

			virtual int getFixedWeight() const = 0;
			virtual void setFixedWeight(int nw) = 0;
			virtual bool isFixedWeightNull() const { return false; }
			virtual void setFixedWeightNull() = 0;
			virtual bool isFixedWeightWritable() const { return true; }

			virtual int getShelfLifeDays() const = 0;
			virtual void setShelfLifeDays(int sld) = 0;
			virtual bool isShelfLifeDaysNull() const { return false; }
			virtual void setShelfLifeDaysNull() = 0;
			virtual bool isShelfLifeDaysWritable() const { return true; }

			virtual int getShelfLifeHours() const = 0;
			virtual void setShelfLifeHours(int slh) = 0;
			virtual bool isShelfLifeHoursNull() const { return false; }
			virtual void setShelfLifeHoursNull() = 0;
			virtual bool isShelfLifeHoursWritable() const { return true; }

			virtual int getShelfLifeMinutes() const = 0;
			virtual void setShelfLifeMinutes(int slm) = 0;
			virtual bool isShelfLifeMinutesNull() const { return false; }
			virtual void setShelfLifeMinutesNull() = 0;
			virtual bool isShelfLifeMinutesWritable() const { return true; }

			virtual int getProductLifeDays() const = 0;
			virtual void setProductLifeDays(int pld) = 0;
			virtual bool isProductLifeDaysNull() const { return false; }
			virtual void setProductLifeDaysNull() = 0;
			virtual bool isProductLifeDaysWritable() const { return true; }

			virtual int getProductLifeHours() const = 0;
			virtual void setProductLifeHours(int plh) = 0;
			virtual bool isProductLifeHoursNull() const { return false; }
			virtual void setProductLifeHoursNull() = 0;
			virtual bool isProductLifeHoursWritable() const { return true; }

			virtual int getProductLifeMinutes() const = 0;
			virtual void setProductLifeMinutes(int plm) = 0;
			virtual bool isProductLifeMinutesNull() const { return false; }
			virtual void setProductLifeMinutesNull() = 0;
			virtual bool isProductLifeMinutesWritable() const { return true; }

			virtual std::string getLabel1() const = 0;
			virtual void setLabel1(const std::string& l1) = 0;
			virtual bool isLabel1Null() const { return false; }
			virtual void setLabel1Null() = 0;
			virtual bool isLabel1Writable() const { return true; }

			virtual std::string getLabel2() const = 0;
			virtual void setLabel2(const std::string& l2) = 0;
			virtual bool isLabel2Null() const { return false; }
			virtual void setLabel2Null() = 0;
			virtual bool isLabel2Writable() const { return true; }

			virtual std::string getLabel3() const = 0;
			virtual void setLabel3(const std::string& l3) = 0;
			virtual bool isLabel3Null() const { return false; }
			virtual void setLabel3Null() = 0;
			virtual bool isLabel3Writable() const { return true; }

			virtual std::string getLabel4() const = 0;
			virtual void setLabel4(const std::string& l4) = 0;
			virtual bool isLabel4Null() const { return false; }
			virtual void setLabel4Null() = 0;
			virtual bool isLabel4Writable() const { return true; }

			virtual std::string getGraphic1() const = 0;
			virtual void setGraphic1(const std::string& g1) = 0;
			virtual bool isGraphic1Null() const { return false; }
			virtual void setGraphic1Null() = 0;
			virtual bool isGraphic1Writable() const { return true; }

			virtual std::string getGraphic2() const = 0;
			virtual void setGraphic2(const std::string& g2) = 0;
			virtual bool isGraphic2Null() const { return false; }
			virtual void setGraphic2Null() = 0;
			virtual bool isGraphic2Writable() const { return true; }

			virtual std::string getGraphic3() const = 0;
			virtual void setGraphic3(const std::string& g3) = 0;
			virtual bool isGraphic3Null() const { return false; }
			virtual void setGraphic3Null() = 0;
			virtual bool isGraphic3Writable() const { return true; }

			virtual std::string getGraphic4() const = 0;
			virtual void setGraphic4(const std::string& g4) = 0;
			virtual bool isGraphic4Null() const { return false; }
			virtual void setGraphic4Null() = 0;
			virtual bool isGraphic4Writable() const { return true; }

			virtual std::string getCoolText() const = 0;
			virtual void setCoolText(const std::string& text) = 0;
			virtual bool isCoolTextNull() const { return false; }
			virtual void setCoolTextNull() = 0;
			virtual bool isCoolTextWritable() const { return true; }

			virtual std::string getCoolTrackingText() const = 0;
			virtual void setCoolTrackingText(const std::string& text) = 0;
			virtual bool isCoolTrackingTextNull() const { return false; }
			virtual void setCoolTrackingTextNull() = 0;
			virtual bool isCoolTrackingTextWritable() const { return true; }

			virtual std::string getOperatorID() const = 0;
			virtual void setOperatorID(const std::string& oper) = 0;
			virtual bool isOperatorIDNull() const { return false; }
			virtual void setOperatorIDNull() = 0;
			virtual bool isOperatorIDWritable() const { return true; }

			virtual std::string getOperatorName() const = 0;
			virtual void setOperatorName(const std::string& name) = 0;
			virtual bool isOperatorNameNull() const { return false; }
			virtual void setOperatorNameNull() = 0;
			virtual bool isOperatorNameWritable() const { return true; }

			virtual std::string getOperatorRFID() const = 0;
			virtual void setOperatorRFID(const std::string& name) = 0;
			virtual bool isOperatorRFIDNull() const { return false; }
			virtual void setOperatorRFIDNull() = 0;
			virtual bool isOperatorRFIDWritable() const { return true; }

			virtual int getOperatorChangeType() const = 0;
			virtual void setOperatorChangeType(int type) = 0;
			virtual bool isOperatorChangeTypeNull() const { return false; }
			virtual void setOperatorChangeTypeNull() = 0;
			virtual bool isOperatorChangeTypeWritable() const { return true; }


			virtual int getOldBarCodeDigit() const = 0;
			virtual void setOldBarCodeDigit(int bcpr) = 0;
			virtual bool isOldBarCodeDigitNull() const { return false; }
			virtual void setOldBarCodeDigitNull() = 0;
			virtual bool isOldBarCodeDigitWritable() const { return true; }

			virtual int getOldUnitPrice() const = 0;
			virtual void setOldUnitPrice(int price) = 0;
			virtual bool isOldUnitPriceNull() const { return false; }
			virtual void setOldUnitPriceNull() = 0;
			virtual bool isOldUnitPriceWritable() const { return true; }

			virtual int getOldFinalPrice() const = 0;
			virtual void setOldFinalPrice(int price) = 0;
			virtual bool isOldFinalPriceNull() const { return false; }
			virtual void setOldFinalPriceNull() = 0;
			virtual bool isOldFinalPriceWritable() const { return true; }

			virtual int getOldTare() const = 0;
			virtual void setOldTare(int t) = 0;
			virtual bool isOldTareNull() const { return false; }
			virtual void setOldTareNull() = 0;
			virtual bool isOldTareWritable() const { return true; }

			virtual int getOldByCount() const = 0;
			virtual void setOldByCount(int bc) = 0;
			virtual bool isOldByCountNull() const { return false; }
			virtual void setOldByCountNull() = 0;
			virtual bool isOldByCountWritable() const { return true; }

			virtual int getOldFixedWeight() const = 0;
			virtual void setOldFixedWeight(int nw) = 0;
			virtual bool isOldFixedWeightNull() const { return false; }
			virtual void setOldFixedWeightNull() = 0;
			virtual bool isOldFixedWeightWritable() const { return true; }

			virtual int getOldShelfLifeDays() const = 0;
			virtual void setOldShelfLifeDays(int sld) = 0;
			virtual bool isOldShelfLifeDaysNull() const { return false; }
			virtual void setOldShelfLifeDaysNull() = 0;
			virtual bool isOldShelfLifeDaysWritable() const { return true; }

			virtual int getOldShelfLifeHours() const = 0;
			virtual void setOldShelfLifeHours(int slh) = 0;
			virtual bool isOldShelfLifeHoursNull() const { return false; }
			virtual void setOldShelfLifeHoursNull() = 0;
			virtual bool isOldShelfLifeHoursWritable() const { return true; }

			virtual int getOldShelfLifeMinutes() const = 0;
			virtual void setOldShelfLifeMinutes(int slm) = 0;
			virtual bool isOldShelfLifeMinutesNull() const { return false; }
			virtual void setOldShelfLifeMinutesNull() = 0;
			virtual bool isOldShelfLifeMinutesWritable() const { return true; }

			virtual int getOldProductLifeDays() const = 0;
			virtual void setOldProductLifeDays(int pld) = 0;
			virtual bool isOldProductLifeDaysNull() const { return false; }
			virtual void setOldProductLifeDaysNull() = 0;
			virtual bool isOldProductLifeDaysWritable() const { return true; }

			virtual int getOldProductLifeHours() const = 0;
			virtual void setOldProductLifeHours(int plh) = 0;
			virtual bool isOldProductLifeHoursNull() const { return false; }
			virtual void setOldProductLifeHoursNull() = 0;
			virtual bool isOldProductLifeHoursWritable() const { return true; }

			virtual int getOldProductLifeMinutes() const = 0;
			virtual void setOldProductLifeMinutes(int plm) = 0;
			virtual bool isOldProductLifeMinutesNull() const { return false; }
			virtual void setOldProductLifeMinutesNull() = 0;
			virtual bool isOldProductLifeMinutesWritable() const { return true; }

			virtual std::string getOldLabel1() const = 0;
			virtual void setOldLabel1(const std::string& l1) = 0;
			virtual bool isOldLabel1Null() const { return false; }
			virtual void setOldLabel1Null() = 0;
			virtual bool isOldLabel1Writable() const { return true; }

			virtual std::string getOldLabel2() const = 0;
			virtual void setOldLabel2(const std::string& l2) = 0;
			virtual bool isOldLabel2Null() const { return false; }
			virtual void setOldLabel2Null() = 0;
			virtual bool isOldLabel2Writable() const { return true; }

			virtual std::string getOldLabel3() const = 0;
			virtual void setOldLabel3(const std::string& l3) = 0;
			virtual bool isOldLabel3Null() const { return false; }
			virtual void setOldLabel3Null() = 0;
			virtual bool isOldLabel3Writable() const { return true; }

			virtual std::string getOldLabel4() const = 0;
			virtual void setOldLabel4(const std::string& l4) = 0;
			virtual bool isOldLabel4Null() const { return false; }
			virtual void setOldLabel4Null() = 0;
			virtual bool isOldLabel4Writable() const { return true; }

			virtual std::string getOldGraphic1() const = 0;
			virtual void setOldGraphic1(const std::string& g1) = 0;
			virtual bool isOldGraphic1Null() const { return false; }
			virtual void setOldGraphic1Null() = 0;
			virtual bool isOldGraphic1Writable() const { return true; }

			virtual std::string getOldGraphic2() const = 0;
			virtual void setOldGraphic2(const std::string& g2) = 0;
			virtual bool isOldGraphic2Null() const { return false; }
			virtual void setOldGraphic2Null() = 0;
			virtual bool isOldGraphic2Writable() const { return true; }

			virtual std::string getOldGraphic3() const = 0;
			virtual void setOldGraphic3(const std::string& g3) = 0;
			virtual bool isOldGraphic3Null() const { return false; }
			virtual void setOldGraphic3Null() = 0;
			virtual bool isOldGraphic3Writable() const { return true; }

			virtual std::string getOldGraphic4() const = 0;
			virtual void setOldGraphic4(const std::string& g4) = 0;
			virtual bool isOldGraphic4Null() const { return false; }
			virtual void setOldGraphic4Null() = 0;
			virtual bool isOldGraphic4Writable() const { return true; }

			virtual std::string getCutTestNumber() const = 0;
			virtual void setCutTestNumber(const std::string& num) = 0;
			virtual bool isCutTestNumberNull() const { return false; }
			virtual void setCutTestNumberNull() = 0;
			virtual bool isCutTestNumberWritable() const { return true; }

			virtual int getPercentOfPrimalWeight() const = 0;
			virtual void setPercentOfPrimalWeight(const int pct) = 0;
			virtual bool isPercentOfPrimalWeightNull() const { return false; }
			virtual void setPercentOfPrimalWeightNull() = 0;
			virtual bool isPercentOfPrimalWeightWritable() const { return true; }

			virtual int getPercentOfPrimalCost() const = 0;
			virtual void setPercentOfPrimalCost(const int pct) = 0;
			virtual bool isPercentOfPrimalCostNull() const { return false; }
			virtual void setPercentOfPrimalCostNull() = 0;
			virtual bool isPercentOfPrimalCostWritable() const { return true; }

			virtual int getModeChange() const = 0;
			virtual void setModeChange(const int modechg) = 0;
			virtual bool isModeChangeNull() const { return false; }
			virtual void setModeChangeNull() = 0;
			virtual bool isModeChangeWritable() const { return true; }

			virtual int getSweetheartWeight() const = 0;
			virtual void setSweetheartWeight(int nw) = 0;
			virtual bool isSweetheartWeightNull() const { return false; }
			virtual void setSweetheartWeightNull() = 0;
			virtual bool isSweetheartWeightWritable() const { return true; }

			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBTRANSACTIONI_H
