#ifndef SQLITETRANSACTION_H
#define SQLITETRANSACTION_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"
#include "CSmartPtr.h"
#include "Bool.h"

#include DBTRANSACTIONI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteTransaction : public DBTransactionI
	{
		public:

			// Types and subtypes
			typedef DBTransactionI interface_type;

			
			// Constructors and destructor
			SqliteTransaction(SqliteRecord* r, DBTransactionI* d = 0);
			~SqliteTransaction();


			// Think about replacing this method with two friend functions from the
			// SqliteTransactionsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_.get(); }


			// Methods from DBTransactionI
			virtual DBTransactionI* clone() const { return new SqliteTransaction(new SqliteRecord(*rec_.get())); }
			virtual const DBTransactionI* getDecorated() const { return decorated_.get(); }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			std::string getScaleID() const { return rec_->getField(SCALEID_FIELD); }
			void setScaleID(const std::string& scaleId) { rec_->setField(SCALEID_FIELD, scaleId); }
			bool isScaleIDNull() const { return rec_->isFieldNull(SCALEID_FIELD); }
			bool isScaleIDWritable() const { return true; }

			std::string getTransactionID() const { return rec_->getField(TRANSACTIONID_FIELD); }
			void setTransactionID(const std::string& id) { rec_->setField(TRANSACTIONID_FIELD, id); }
			bool isTransactionIDNull() const { return rec_->isFieldNull(TRANSACTIONID_FIELD); }
			bool isTransactionIDWritable() const { return true; }

			int getTransactionType() const { return rec_->getField(TRANSACTIONTYPE_FIELD); }
			void setTransactionType(int type) { rec_->setField(TRANSACTIONTYPE_FIELD, type); }
			bool isTransactionTypeNull() const { return rec_->isFieldNull(TRANSACTIONTYPE_FIELD); }
			void setTransactionTypeNull() { rec_->setFieldNull(TRANSACTIONTYPE_FIELD); }
			bool isTransactionTypeWritable() const { return true; }

			std::string getTransactionGroup() const { return rec_->getField(TRANSACTIONGROUP_FIELD); }
			void setTransactionGroup(const std::string& group) { rec_->setField(TRANSACTIONGROUP_FIELD, group); }
			bool isTransactionGroupNull() const { return rec_->isFieldNull(TRANSACTIONGROUP_FIELD); }
			void setTransactionGroupNull() { rec_->setFieldNull(TRANSACTIONGROUP_FIELD); }
			bool isTransactionGroupWritable() const { return true; }

			std::string getTransactionDateTime() const { return rec_->getField(TRANSACTIONDATETIME_FIELD); }
			void setTransactionDateTime(const std::string& tdt) { rec_->setField(TRANSACTIONDATETIME_FIELD, tdt); }
			bool isTransactionDateTimeNull() const { return rec_->isFieldNull(TRANSACTIONDATETIME_FIELD); }
			void setTransactionDateTimeNull() { rec_->setFieldNull(TRANSACTIONDATETIME_FIELD); }
			bool isTransactionDateTimeWritable() const { return true; }

			int getTransactionVoid() const { return rec_->getField(TRANSACTIONVOID_FIELD); }
			void setTransactionVoid(int flag) { rec_->setField(TRANSACTIONVOID_FIELD, flag); }
			bool isTransactionVoidNull() const { return rec_->isFieldNull(TRANSACTIONVOID_FIELD); }
			void setTransactionVoidNull() { rec_->setFieldNull(TRANSACTIONVOID_FIELD); }
			bool isTransactionVoidWritable() const { return true; }

			int getTransactionAffectsProduction() const { return rec_->getField(AFFECTSPRODUCTION_FIELD); }
			void setTransactionAffectsProduction(int flag) { rec_->setField(AFFECTSPRODUCTION_FIELD, flag); }
			bool isTransactionAffectsProductionNull() const { return rec_->isFieldNull(AFFECTSPRODUCTION_FIELD); }
			void setTransactionAffectsProductionNull() { rec_->setFieldNull(AFFECTSPRODUCTION_FIELD); }
			bool isTransactionAffectsProductionWritable() const { return true; }

			int getTransactionReadCleared() const { return rec_->getField(READANDCLEAR_FIELD); }
			void setTransactionReadCleared(int flag) { rec_->setField(READANDCLEAR_FIELD, flag); }
			bool isTransactionReadClearedNull() const { return rec_->isFieldNull(READANDCLEAR_FIELD); }
			void setTransactionReadClearedNull() { rec_->setFieldNull(READANDCLEAR_FIELD); }
			bool isTransactionReadClearedWritable() const { return true; }

			int getTransactionPackages() const { return rec_->getField(NUMBERPACKAGES_FIELD); }
			void setTransactionPackages(int packs) { rec_->setField(NUMBERPACKAGES_FIELD, packs); }
			bool isTransactionPackagesNull() const { return rec_->isFieldNull(NUMBERPACKAGES_FIELD); }
			void setTransactionPackagesNull() { rec_->setFieldNull(NUMBERPACKAGES_FIELD); }
			bool isTransactionPackagesWritable() const { return true; }

			bool getTransactionUsedSecurityLabel() const { return util_string::stringtobool(rec_->getField(USEDSECURITYLABEL_FIELD)); }
			void setTransactionUsedSecurityLabel(bool value) { rec_->setField(USEDSECURITYLABEL_FIELD, Bool::toString(value)); }
			bool isTransactionUsedSecurityLabelNull() const { return rec_->isFieldNull(USEDSECURITYLABEL_FIELD); }
			void setTransactionUsedSecurityLabelNull() { rec_->setFieldNull(USEDSECURITYLABEL_FIELD); }
			bool isTransactionUsedSecurityLabelWritable() const { return true; }

			bool getTransactionCOOLFlag() const { return util_string::stringtobool(rec_->getField(COOLFLAG_FIELD)); }
			void setTransactionCOOLFlag(bool value) { rec_->setField(COOLFLAG_FIELD, Bool::toString(value)); }
			bool isTransactionCOOLFlagNull() const { return rec_->isFieldNull(COOLFLAG_FIELD); }
			void setTransactionCOOLFlagNull() { rec_->setFieldNull(COOLFLAG_FIELD); }
			bool isTransactionCOOLFlagWritable() const { return true; }

			int getTransactionTotalValue() const { return rec_->getField(TOTALVALUE_FIELD); }
			void setTransactionTotalValue(int value) { rec_->setField(TOTALVALUE_FIELD, value); }
			bool isTransactionTotalValueNull() const { return rec_->isFieldNull(TOTALVALUE_FIELD); }
			void setTransactionTotalValueNull() { rec_->setFieldNull(TOTALVALUE_FIELD); }
			bool isTransactionTotalValueWritable() const { return true; }

			int getTransactionOldTotalValue() const { return rec_->getField(OLDTOTALVALUE_FIELD); }
			void setTransactionOldTotalValue(int value) { rec_->setField(OLDTOTALVALUE_FIELD, value); }
			bool isTransactionOldTotalValueNull() const { return rec_->isFieldNull(OLDTOTALVALUE_FIELD); }
			void setTransactionOldTotalValueNull() { rec_->setFieldNull(OLDTOTALVALUE_FIELD); }
			bool isTransactionOldTotalValueWritable() const { return true; }

			virtual std::string getDeptNumber() const { return rec_->getField(DEPTNUM_FIELD); }
			virtual void setDeptNumber(const std::string& number) { rec_->setField(DEPTNUM_FIELD, number); }
			virtual bool isDeptNumberNull() const { return rec_->isFieldNull(DEPTNUM_FIELD); }
			virtual void setDeptNumberNull() { rec_->setFieldNull(DEPTNUM_FIELD); }
			virtual bool isDeptNumberWritable() const { return true; }

			std::string getProductNumber() const { return rec_->getField(PRODUCTNUMBER_FIELD); }
			void setProductNumber(const std::string& plu) { rec_->setField(PRODUCTNUMBER_FIELD, plu); }
			bool isProductNumberNull() const { return rec_->isFieldNull(PRODUCTNUMBER_FIELD); }
			void setProductNumberNull() { rec_->setFieldNull(PRODUCTNUMBER_FIELD); }
			bool isProductNumberWritable() const { return true; }

			std::string getReportingClass() const { return rec_->getField(REPORTINGCLASS_FIELD); }
			void setReportingClass(const std::string& rc) { rec_->setField(REPORTINGCLASS_FIELD, rc); }
			bool isReportingClassNull() const { return rec_->isFieldNull(REPORTINGCLASS_FIELD); }
			void setReportingClassNull() { rec_->setFieldNull(REPORTINGCLASS_FIELD); }
			bool isReportingClassWritable() const { return true; }

			int getProductType() const { return rec_->getField(PRODUCTTYPE_FIELD); }
			void setProductType(int type) { rec_->setField(PRODUCTTYPE_FIELD, type); }
			bool isProductTypeNull() const { return rec_->isFieldNull(PRODUCTTYPE_FIELD); }
			void setProductTypeNull() { rec_->setFieldNull(PRODUCTTYPE_FIELD); }
			bool isProductTypeWritable() const { return true; }

			int getProductionPlanningType() const { return rec_->getField(PRODUCTIONPLANNINGTYPE_FIELD); }
			void setProductionPlanningType(int type) { rec_->setField(PRODUCTIONPLANNINGTYPE_FIELD, type); }
			bool isProductionPlanningTypeNull() const { return rec_->isFieldNull(PRODUCTIONPLANNINGTYPE_FIELD); }
			void setProductionPlanningTypeNull() { rec_->setFieldNull(PRODUCTIONPLANNINGTYPE_FIELD); }
			bool isProductionPlanningTypeWritable() const { return true; }

			std::string getProductDescription() const { return rec_->getField(PRODUCTDESCRIPTION_FIELD); }
			void setProductDescription(const std::string& desc) { rec_->setField(PRODUCTDESCRIPTION_FIELD, desc); }
			bool isProductDescriptionNull() const { return rec_->isFieldNull(PRODUCTDESCRIPTION_FIELD); }
			void setProductDescriptionNull() { rec_->setFieldNull(PRODUCTDESCRIPTION_FIELD); }
			bool isProductDescriptionWritable() const { return true; }

			int getPriceFactor() const { return rec_->getField(PRICEFACTOR_FIELD); }
			void setPriceFactor(int pf) { rec_->setField(PRICEFACTOR_FIELD, pf); }
			bool isPriceFactorNull() const { return rec_->isFieldNull(PRICEFACTOR_FIELD); }
			void setPriceFactorNull() { rec_->setFieldNull(PRICEFACTOR_FIELD); }
			bool isPriceFactorWritable() const { return true; }

			int getBarCodePrefix() const { return rec_->getField(BARCODEPREFIX_FIELD); }
			void setBarCodePrefix(int bcpr) { rec_->setField(BARCODEPREFIX_FIELD, bcpr); }
			bool isBarCodePrefixNull() const { return rec_->isFieldNull(BARCODEPREFIX_FIELD); }
			void setBarCodePrefixNull() { rec_->setFieldNull(BARCODEPREFIX_FIELD); }
			bool isBarCodePrefixWritable() const { return true; }

			std::string getBarCodeNumber() const { return rec_->getField(BARCODENUMBER_FIELD); }
			void setBarCodeNumber(const std::string& barcode) { rec_->setField(BARCODENUMBER_FIELD, barcode); }
			bool isBarCodeNumberNull() const { return rec_->isFieldNull(BARCODENUMBER_FIELD); }
			void setBarCodeNumberNull() { rec_->setFieldNull(BARCODENUMBER_FIELD); }
			bool isBarCodeNumberWritable() const { return true; }

			int getUnitPrice() const { return rec_->getField(UNITPRICE_FIELD); }
			void setUnitPrice(int price) { rec_->setField(UNITPRICE_FIELD, price); }
			bool isUnitPriceNull() const { return rec_->isFieldNull(UNITPRICE_FIELD); }
			void setUnitPriceNull() { rec_->setFieldNull(UNITPRICE_FIELD); }
			bool isUnitPriceWritable() const { return true; }

			int getExceptionPrice() const { return rec_->getField(EXCEPTIONPRICE_FIELD); }
			void setExceptionPrice(int xp) { rec_->setField(EXCEPTIONPRICE_FIELD, xp); }
			bool isExceptionPriceNull() const { return rec_->isFieldNull(EXCEPTIONPRICE_FIELD); }
			void setExceptionPriceNull() { rec_->setFieldNull(EXCEPTIONPRICE_FIELD); }
			bool isExceptionPriceWritable() const { return true; }

			int getFinalPrice() const { return rec_->getField(FINALPRICE_FIELD); }
			void setFinalPrice(int price) { rec_->setField(FINALPRICE_FIELD, price); }
			bool isFinalPriceNull() const { return rec_->isFieldNull(FINALPRICE_FIELD); }
			void setFinalPriceNull() { rec_->setFieldNull(FINALPRICE_FIELD); }
			bool isFinalPriceWritable() const { return true; }

			int getTare() const { return rec_->getField(TARE_FIELD); }
			void setTare(int t) { rec_->setField(TARE_FIELD, t); }
			bool isTareNull() const { return rec_->isFieldNull(TARE_FIELD); }
			void setTareNull() { rec_->setFieldNull(TARE_FIELD); }
			bool isTareWritable() const { return true; }

			int getProportionalTare() const { return rec_->getField(PROPORTIONALTARE_FIELD); }
			void setProportionalTare(int pt) { rec_->setField(PROPORTIONALTARE_FIELD, pt); }
			bool isProportionalTareNull() const { return rec_->isFieldNull(PROPORTIONALTARE_FIELD); }
			void setProportionalTareNull() { rec_->setFieldNull(PROPORTIONALTARE_FIELD); }
			bool isProportionalTareWritable() const { return true; }

			int getFinalTare() const { return rec_->getField(FINALTARE_FIELD); }
			void setFinalTare(int t) { rec_->setField(FINALTARE_FIELD, t); }
			bool isFinalTareNull() const { return rec_->isFieldNull(FINALTARE_FIELD); }
			void setFinalTareNull() { rec_->setFieldNull(FINALTARE_FIELD); }
			bool isFinalTareWritable() const { return true; }

			int getByCount() const { return rec_->getField(BYCOUNT_FIELD); }
			void setByCount(int bc) { rec_->setField(BYCOUNT_FIELD, bc); }
			bool isByCountNull() const { return rec_->isFieldNull(BYCOUNT_FIELD); }
			void setByCountNull() { rec_->setFieldNull(BYCOUNT_FIELD); }
			bool isByCountWritable() const { return true; }

			int getFinalNetWeight() const { return rec_->getField(FINALNETWEIGHT_FIELD); }
			void setFinalNetWeight(int nw) { rec_->setField(FINALNETWEIGHT_FIELD, nw); }
			bool isFinalNetWeightNull() const { return rec_->isFieldNull(FINALNETWEIGHT_FIELD); }
			void setFinalNetWeightNull() { rec_->setFieldNull(FINALNETWEIGHT_FIELD); }
			bool isFinalNetWeightWritable() const { return true; }

			int getFixedWeight() const { return rec_->getField(FIXEDWEIGHT_FIELD); }
			void setFixedWeight(int fw) { rec_->setField(FIXEDWEIGHT_FIELD, fw); }
			bool isFixedWeightNull() const { return rec_->isFieldNull(FIXEDWEIGHT_FIELD); }
			void setFixedWeightNull() { rec_->setFieldNull(FIXEDWEIGHT_FIELD); }
			bool isFixedWeightWritable() const { return true; }

			int getShelfLifeDays() const { return rec_->getField(SHELFLIFEDAYS_FIELD); }
			void setShelfLifeDays(int sld) { rec_->setField(SHELFLIFEDAYS_FIELD, sld); }
			bool isShelfLifeDaysNull() const { return rec_->isFieldNull(SHELFLIFEDAYS_FIELD); }
			void setShelfLifeDaysNull() { rec_->setFieldNull(SHELFLIFEDAYS_FIELD); }
			bool isShelfLifeDaysWritable() const { return true; }

			int getShelfLifeHours() const { return rec_->getField(SHELFLIFEHOURS_FIELD); }
			void setShelfLifeHours(int slh) { rec_->setField(SHELFLIFEHOURS_FIELD, slh); }
			bool isShelfLifeHoursNull() const { return rec_->isFieldNull(SHELFLIFEHOURS_FIELD); }
			void setShelfLifeHoursNull() { rec_->setFieldNull(SHELFLIFEHOURS_FIELD); }
			bool isShelfLifeHoursWritable() const { return true; }

			int getShelfLifeMinutes() const { return rec_->getField(SHELFLIFEMINUTES_FIELD); }
			void setShelfLifeMinutes(int slm) { rec_->setField(SHELFLIFEMINUTES_FIELD, slm); }
			bool isShelfLifeMinutesNull() const { return rec_->isFieldNull(SHELFLIFEMINUTES_FIELD); }
			void setShelfLifeMinutesNull() { rec_->setFieldNull(SHELFLIFEMINUTES_FIELD); }
			bool isShelfLifeMinutesWritable() const { return true; }

			int getProductLifeDays() const { return rec_->getField(PRODUCTLIFEDAYS_FIELD); }
			void setProductLifeDays(int pld) { rec_->setField(PRODUCTLIFEDAYS_FIELD, pld); }
			bool isProductLifeDaysNull() const { return rec_->isFieldNull(PRODUCTLIFEDAYS_FIELD); }
			void setProductLifeDaysNull() { rec_->setFieldNull(PRODUCTLIFEDAYS_FIELD); }
			bool isProductLifeDaysWritable() const { return true; }

			int getProductLifeHours() const { return rec_->getField(PRODUCTLIFEHOURS_FIELD); }
			void setProductLifeHours(int plh) { rec_->setField(PRODUCTLIFEHOURS_FIELD, plh); }
			bool isProductLifeHoursNull() const { return rec_->isFieldNull(PRODUCTLIFEHOURS_FIELD); }
			void setProductLifeHoursNull() { rec_->setFieldNull(PRODUCTLIFEHOURS_FIELD); }
			bool isProductLifeHoursWritable() const { return true; }

			int getProductLifeMinutes() const { return rec_->getField(PRODUCTLIFEMINUTES_FIELD); }
			void setProductLifeMinutes(int plm) { rec_->setField(PRODUCTLIFEMINUTES_FIELD, plm); }
			bool isProductLifeMinutesNull() const { return rec_->isFieldNull(PRODUCTLIFEMINUTES_FIELD); }
			void setProductLifeMinutesNull() { rec_->setFieldNull(PRODUCTLIFEMINUTES_FIELD); }
			bool isProductLifeMinutesWritable() const { return true; }

			std::string getLabel1() const { return rec_->getField(LABEL1_FIELD); }
			void setLabel1(const std::string& l1) { rec_->setField(LABEL1_FIELD, l1); }
			bool isLabel1Null() const { return rec_->isFieldNull(LABEL1_FIELD); }
			void setLabel1Null() { rec_->setFieldNull(LABEL1_FIELD); }
			bool isLabel1Writable() const { return true; }

			std::string getLabel2() const { return rec_->getField(LABEL2_FIELD); }
			void setLabel2(const std::string& l2) { rec_->setField(LABEL2_FIELD, l2); }
			bool isLabel2Null() const { return rec_->isFieldNull(LABEL2_FIELD); }
			void setLabel2Null() { rec_->setFieldNull(LABEL2_FIELD); }
			bool isLabel2Writable() const { return true; }

			std::string getLabel3() const { return rec_->getField(LABEL3_FIELD); }
			void setLabel3(const std::string& l3) { rec_->setField(LABEL3_FIELD, l3); }
			bool isLabel3Null() const { return rec_->isFieldNull(LABEL3_FIELD); }
			void setLabel3Null() { rec_->setFieldNull(LABEL3_FIELD); }
			bool isLabel3Writable() const { return true; }

			std::string getLabel4() const { return rec_->getField(LABEL4_FIELD); }
			void setLabel4(const std::string& l4) { rec_->setField(LABEL4_FIELD, l4); }
			bool isLabel4Null() const { return rec_->isFieldNull(LABEL4_FIELD); }
			void setLabel4Null() { rec_->setFieldNull(LABEL4_FIELD); }
			bool isLabel4Writable() const { return true; }

			std::string getOperatorID() const { return rec_->getField(OPERATORNUMBER_FIELD); }
			void setOperatorID(const std::string& oper) { rec_->setField(OPERATORNUMBER_FIELD, oper); }
			bool isOperatorIDNull() const { return rec_->isFieldNull(OPERATORNUMBER_FIELD); }
			void setOperatorIDNull() { rec_->setFieldNull(OPERATORNUMBER_FIELD); }
			bool isOperatorIDWritable() const { return true; }

			std::string getOperatorRFID() const { return rec_->getField(OPERATORRFID_FIELD); }
			void setOperatorRFID(const std::string& name) { rec_->setField(OPERATORRFID_FIELD, name); }
			bool isOperatorRFIDNull() const { return rec_->isFieldNull(OPERATORRFID_FIELD); }
			void setOperatorRFIDNull() { rec_->setFieldNull(OPERATORRFID_FIELD); }
			bool isOperatorRFIDWritable() const { return true; }

			std::string getOperatorName() const { return rec_->getField(OPERATORNAME_FIELD); }
			void setOperatorName(const std::string& name) { rec_->setField(OPERATORNAME_FIELD, name); }
			bool isOperatorNameNull() const { return rec_->isFieldNull(OPERATORNAME_FIELD); }
			void setOperatorNameNull() { rec_->setFieldNull(OPERATORNAME_FIELD); }
			bool isOperatorNameWritable() const { return true; }

			int getOperatorChangeType() const { return rec_->getField(OPERATORCHANGETYPE_FIELD); }
			void setOperatorChangeType(int type) { rec_->setField(OPERATORCHANGETYPE_FIELD, type); }
			bool isOperatorChangeTypeNull() const { return rec_->isFieldNull(OPERATORCHANGETYPE_FIELD); }
			void setOperatorChangeTypeNull() { rec_->setFieldNull(OPERATORCHANGETYPE_FIELD); }
			bool isOperatorChangeTypeWritable() const { return true; }

			std::string getGraphic1() const { return rec_->getField(GRAPHIC1_FIELD); }
			void setGraphic1(const std::string& g1) { rec_->setField(GRAPHIC1_FIELD, g1); }
			bool isGraphic1Null() const { return rec_->isFieldNull(GRAPHIC1_FIELD); }
			void setGraphic1Null() { rec_->setFieldNull(GRAPHIC1_FIELD); }
			bool isGraphic1Writable() const { return true; }

			std::string getGraphic2() const { return rec_->getField(GRAPHIC2_FIELD); }
			void setGraphic2(const std::string& g2) { rec_->setField(GRAPHIC2_FIELD, g2); }
			bool isGraphic2Null() const { return rec_->isFieldNull(GRAPHIC2_FIELD); }
			void setGraphic2Null() { rec_->setFieldNull(GRAPHIC2_FIELD); }
			bool isGraphic2Writable() const { return true; }

			std::string getGraphic3() const { return rec_->getField(GRAPHIC3_FIELD); }
			void setGraphic3(const std::string& g3) { rec_->setField(GRAPHIC3_FIELD, g3); }
			bool isGraphic3Null() const { return rec_->isFieldNull(GRAPHIC3_FIELD); }
			void setGraphic3Null() { rec_->setFieldNull(GRAPHIC3_FIELD); }
			bool isGraphic3Writable() const { return true; }

			std::string getGraphic4() const { return rec_->getField(GRAPHIC4_FIELD); }
			void setGraphic4(const std::string& g4) { rec_->setField(GRAPHIC4_FIELD, g4); }
			bool isGraphic4Null() const { return rec_->isFieldNull(GRAPHIC4_FIELD); }
			void setGraphic4Null() { rec_->setFieldNull(GRAPHIC4_FIELD); }
			bool isGraphic4Writable() const { return true; }

			std::string getCoolText() const { return rec_->getField(COOLTEXT_FIELD); }
			void setCoolText(const std::string& text) { rec_->setField(COOLTEXT_FIELD, text); }
			bool isCoolTextNull() const { return rec_->isFieldNull(COOLTEXT_FIELD); }
			void setCoolTextNull() { rec_->setFieldNull(COOLTEXT_FIELD); }
			bool isCoolTextWritable() const { return true; }

			std::string getCoolTrackingText() const { return rec_->getField(COOLTRACKINGTEXT_FIELD); }
			void setCoolTrackingText(const std::string& text) { rec_->setField(COOLTRACKINGTEXT_FIELD, text); }
			bool isCoolTrackingTextNull() const { return rec_->isFieldNull(COOLTRACKINGTEXT_FIELD); }
			void setCoolTrackingTextNull() { rec_->setFieldNull(COOLTRACKINGTEXT_FIELD); }
			bool isCoolTrackingTextWritable() const { return true; }

			std::string getCutTestNumber() const { return rec_->getField(CUTTESTNUMBER_FIELD); }
			void setCutTestNumber(const std::string& num) { rec_->setField(CUTTESTNUMBER_FIELD, num); }
			bool isCutTestNumberNull() const { return rec_->isFieldNull(CUTTESTNUMBER_FIELD); }
			void setCutTestNumberNull() { rec_->setFieldNull(CUTTESTNUMBER_FIELD); }
			bool isCutTestNumberWritable() const { return true; }

			int getPercentOfPrimalWeight() const { return rec_->getField(PERCENTOFPRIMALWEIGHT_FIELD); }
			void setPercentOfPrimalWeight(const int pct) { rec_->setField(PERCENTOFPRIMALWEIGHT_FIELD, pct); }
			bool isPercentOfPrimalWeightNull() const { return rec_->isFieldNull(PERCENTOFPRIMALWEIGHT_FIELD); }
			void setPercentOfPrimalWeightNull() { rec_->setFieldNull(PERCENTOFPRIMALWEIGHT_FIELD); }
			bool isPercentOfPrimalWeightWritable() const { return true; }

			int getPercentOfPrimalCost() const { return rec_->getField (PERCENTOFPRIMALCOST_FIELD); }
			void setPercentOfPrimalCost(const int pct) { rec_->setField(PERCENTOFPRIMALCOST_FIELD, pct); }
			bool isPercentOfPrimalCostNull() const { return rec_->isFieldNull(PERCENTOFPRIMALCOST_FIELD); }
			void setPercentOfPrimalCostNull() { rec_->setFieldNull(PERCENTOFPRIMALCOST_FIELD); }
			bool isPercentOfPrimalCostWritable() const { return true; }

			int getModeChange() const { return rec_->getField(MODECHANGE_FIELD); }
			void setModeChange(const int num) { rec_->setField(MODECHANGE_FIELD, num); }
			bool isModeChangeNull() const { return rec_->isFieldNull(MODECHANGE_FIELD); }
			void setModeChangeNull() { rec_->setFieldNull(MODECHANGE_FIELD); }
			bool isModeChangeWritable() const { return true; }

			int getOldBarCodeDigit() const { return rec_->getField(OLDBARCODEDIGIT_FIELD); }
			void setOldBarCodeDigit(int bcpr) { rec_->setField(OLDBARCODEDIGIT_FIELD, bcpr); }
			bool isOldBarCodeDigitNull() const { return rec_->isFieldNull(OLDBARCODEDIGIT_FIELD); }
			void setOldBarCodeDigitNull() { rec_->setFieldNull(OLDBARCODEDIGIT_FIELD); }
			bool isOldBarCodeDigitWritable() const { return true; }

			int getOldUnitPrice() const { return rec_->getField(OLDUNITPRICE_FIELD); }
			void setOldUnitPrice(int price) { rec_->setField(OLDUNITPRICE_FIELD, price); }
			bool isOldUnitPriceNull() const { return rec_->isFieldNull(OLDUNITPRICE_FIELD); }
			void setOldUnitPriceNull() { rec_->setFieldNull(OLDUNITPRICE_FIELD); }
			bool isOldUnitPriceWritable() const { return true; }

			int getOldFinalPrice() const { return rec_->getField(OLDFINALPRICE_FIELD); }
			void setOldFinalPrice(int price) { rec_->setField(OLDFINALPRICE_FIELD, price); }
			bool isOldFinalPriceNull() const { return rec_->isFieldNull(OLDFINALPRICE_FIELD); }
			void setOldFinalPriceNull() { rec_->setFieldNull(OLDFINALPRICE_FIELD); }
			bool isOldFinalPriceWritable() const { return true; }

			int getOldTare() const { return rec_->getField(OLDTARE_FIELD); }
			void setOldTare(int t) { rec_->setField(OLDTARE_FIELD, t); }
			bool isOldTareNull() const { return rec_->isFieldNull(OLDTARE_FIELD); }
			void setOldTareNull() { rec_->setFieldNull(OLDTARE_FIELD); }
			bool isOldTareWritable() const { return true; }

			int getOldByCount() const { return rec_->getField(OLDBYCOUNT_FIELD); }
			void setOldByCount(int bc) { rec_->setField(OLDBYCOUNT_FIELD, bc); }
			bool isOldByCountNull() const { return rec_->isFieldNull(OLDBYCOUNT_FIELD); }
			void setOldByCountNull() { rec_->setFieldNull(OLDBYCOUNT_FIELD); }
			bool isOldByCountWritable() const { return true; }

			int getOldFixedWeight() const { return rec_->getField(OLDFIXEDWEIGHT_FIELD); }
			void setOldFixedWeight(int nw) { rec_->setField(OLDFIXEDWEIGHT_FIELD, nw); }
			bool isOldFixedWeightNull() const { return rec_->isFieldNull(OLDFIXEDWEIGHT_FIELD); }
			void setOldFixedWeightNull() { rec_->setFieldNull(OLDFIXEDWEIGHT_FIELD); }
			bool isOldFixedWeightWritable() const { return true; }

			int getOldShelfLifeDays() const { return rec_->getField(OLDSHELFLIFEDAYS_FIELD); }
			void setOldShelfLifeDays(int sld) { rec_->setField(OLDSHELFLIFEDAYS_FIELD, sld); }
			bool isOldShelfLifeDaysNull() const { return rec_->isFieldNull(OLDSHELFLIFEDAYS_FIELD); }
			void setOldShelfLifeDaysNull() { rec_->setFieldNull(OLDSHELFLIFEDAYS_FIELD); }
			bool isOldShelfLifeDaysWritable() const { return true; }

			int getOldShelfLifeHours() const { return rec_->getField(OLDSHELFLIFEHOURS_FIELD); }
			void setOldShelfLifeHours(int slh) { rec_->setField(OLDSHELFLIFEHOURS_FIELD, slh); }
			bool isOldShelfLifeHoursNull() const { return rec_->isFieldNull(OLDSHELFLIFEHOURS_FIELD); }
			void setOldShelfLifeHoursNull() { rec_->setFieldNull(OLDSHELFLIFEHOURS_FIELD); }
			bool isOldShelfLifeHoursWritable() const { return true; }

			int getOldShelfLifeMinutes() const { return rec_->getField(OLDSHELFLIFEMINUTES_FIELD); }
			void setOldShelfLifeMinutes(int slm) { rec_->setField(OLDSHELFLIFEMINUTES_FIELD, slm); }
			bool isOldShelfLifeMinutesNull() const { return rec_->isFieldNull(OLDSHELFLIFEMINUTES_FIELD); }
			void setOldShelfLifeMinutesNull() { rec_->setFieldNull(OLDSHELFLIFEMINUTES_FIELD); }
			bool isOldShelfLifeMinutesWritable() const { return true; }

			int getOldProductLifeDays() const { return rec_->getField(OLDPRODUCTLIFEDAYS_FIELD); }
			void setOldProductLifeDays(int pld) { rec_->setField(OLDPRODUCTLIFEDAYS_FIELD, pld); }
			bool isOldProductLifeDaysNull() const { return rec_->isFieldNull(OLDPRODUCTLIFEDAYS_FIELD); }
			void setOldProductLifeDaysNull() { rec_->setFieldNull(OLDPRODUCTLIFEDAYS_FIELD); }
			bool isOldProductLifeDaysWritable() const { return true; }

			int getOldProductLifeHours() const { return rec_->getField(OLDPRODUCTLIFEHOURS_FIELD); }
			void setOldProductLifeHours(int plh) { rec_->setField(OLDPRODUCTLIFEHOURS_FIELD, plh); }
			bool isOldProductLifeHoursNull() const { return rec_->isFieldNull(OLDPRODUCTLIFEHOURS_FIELD); }
			void setOldProductLifeHoursNull() { rec_->setFieldNull(OLDPRODUCTLIFEHOURS_FIELD); }
			bool isOldProductLifeHoursWritable() const { return true; }

			int getOldProductLifeMinutes() const { return rec_->getField(OLDPRODUCTLIFEMINUTES_FIELD); }
			void setOldProductLifeMinutes(int plm) { rec_->setField(OLDPRODUCTLIFEMINUTES_FIELD, plm); }
			bool isOldProductLifeMinutesNull() const { return rec_->isFieldNull(OLDPRODUCTLIFEMINUTES_FIELD); }
			void setOldProductLifeMinutesNull() { rec_->setFieldNull(OLDPRODUCTLIFEMINUTES_FIELD); }
			bool isOldProductLifeMinutesWritable() const { return true; }

			std::string getOldLabel1() const { return rec_->getField(OLDLABEL1_FIELD); }
			void setOldLabel1(const std::string& l1) { rec_->setField(OLDLABEL1_FIELD, l1); }
			bool isOldLabel1Null() const { return rec_->isFieldNull(OLDLABEL1_FIELD); }
			void setOldLabel1Null() { rec_->setFieldNull(OLDLABEL1_FIELD); }
			bool isOldLabel1Writable() const { return true; }

			std::string getOldLabel2() const { return rec_->getField(OLDLABEL2_FIELD); }
			void setOldLabel2(const std::string& l2) { rec_->setField(OLDLABEL2_FIELD, l2); }
			bool isOldLabel2Null() const { return rec_->isFieldNull(OLDLABEL2_FIELD); }
			void setOldLabel2Null() { rec_->setFieldNull(OLDLABEL2_FIELD); }
			bool isOldLabel2Writable() const { return true; }

			std::string getOldLabel3() const { return rec_->getField(OLDLABEL3_FIELD); }
			void setOldLabel3(const std::string& l3) { rec_->setField(OLDLABEL3_FIELD, l3); }
			bool isOldLabel3Null() const { return rec_->isFieldNull(OLDLABEL3_FIELD); }
			void setOldLabel3Null() { rec_->setFieldNull(OLDLABEL3_FIELD); }
			bool isOldLabel3Writable() const { return true; }

			std::string getOldLabel4() const { return rec_->getField(OLDLABEL4_FIELD); }
			void setOldLabel4(const std::string& l4) { rec_->setField(OLDLABEL4_FIELD, l4); }
			bool isOldLabel4Null() const { return rec_->isFieldNull(OLDLABEL4_FIELD); }
			void setOldLabel4Null() { rec_->setFieldNull(OLDLABEL4_FIELD); }
			bool isOldLabel4Writable() const { return true; }

			std::string getOldGraphic1() const { return rec_->getField(OLDGRAPHIC1_FIELD); }
			void setOldGraphic1(const std::string& g1) { rec_->setField(OLDGRAPHIC1_FIELD, g1); }
			bool isOldGraphic1Null() const { return rec_->isFieldNull(OLDGRAPHIC1_FIELD); }
			void setOldGraphic1Null() { rec_->setFieldNull(OLDGRAPHIC1_FIELD); }
			bool isOldGraphic1Writable() const { return true; }

			std::string getOldGraphic2() const { return rec_->getField(OLDGRAPHIC2_FIELD); }
			void setOldGraphic2(const std::string& g2) { rec_->setField(OLDGRAPHIC2_FIELD, g2); }
			bool isOldGraphic2Null() const { return rec_->isFieldNull(OLDGRAPHIC2_FIELD); }
			void setOldGraphic2Null() { rec_->setFieldNull(OLDGRAPHIC2_FIELD); }
			bool isOldGraphic2Writable() const { return true; }

			std::string getOldGraphic3() const { return rec_->getField(OLDGRAPHIC3_FIELD); }
			void setOldGraphic3(const std::string& g3) { rec_->setField(OLDGRAPHIC3_FIELD, g3); }
			bool isOldGraphic3Null() const { return rec_->isFieldNull(OLDGRAPHIC3_FIELD); }
			void setOldGraphic3Null() { rec_->setFieldNull(OLDGRAPHIC3_FIELD); }
			bool isOldGraphic3Writable() const { return true; }

			std::string getOldGraphic4() const { return rec_->getField(OLDGRAPHIC4_FIELD); }
			void setOldGraphic4(const std::string& g4) { rec_->setField(OLDGRAPHIC4_FIELD, g4); }
			bool isOldGraphic4Null() const { return rec_->isFieldNull(OLDGRAPHIC4_FIELD); }
			void setOldGraphic4Null() { rec_->setFieldNull(OLDGRAPHIC4_FIELD); }
			bool isOldGraphic4Writable() const { return true; }
			
			int getSweetheartWeight() const { return rec_->getField(SWEETHEARTWEIGHT_FIELD); }
			void setSweetheartWeight(int w) { rec_->setField(SWEETHEARTWEIGHT_FIELD, w); }
			bool isSweetheartWeightNull() const { return rec_->isFieldNull(SWEETHEARTWEIGHT_FIELD); }
			void setSweetheartWeightNull() { rec_->setFieldNull(SWEETHEARTWEIGHT_FIELD); }
			bool isSweetheartWeightWritable() const { return true; }

			bool isRecordValid() { return rec_->isValid(); }

			static const std::string SCALEID_FIELD;
			static const std::string TRANSACTIONID_FIELD;
			static const std::string TRANSACTIONTYPE_FIELD;
			static const std::string TRANSACTIONGROUP_FIELD;
			static const std::string TRANSACTIONDATETIME_FIELD;
			static const std::string TRANSACTIONVOID_FIELD;
			static const std::string AFFECTSPRODUCTION_FIELD;
			static const std::string READANDCLEAR_FIELD;
			static const std::string NUMBERPACKAGES_FIELD;
			static const std::string USEDSECURITYLABEL_FIELD;
			static const std::string COOLFLAG_FIELD;
			static const std::string TOTALVALUE_FIELD;
			static const std::string OLDTOTALVALUE_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string PRODUCTNUMBER_FIELD;
			static const std::string REPORTINGCLASS_FIELD;
			static const std::string PRODUCTTYPE_FIELD;
			static const std::string PRODUCTIONPLANNINGTYPE_FIELD;
			static const std::string PRODUCTDESCRIPTION_FIELD;
			static const std::string PRICEFACTOR_FIELD;
			static const std::string BARCODEPREFIX_FIELD;
			static const std::string BARCODENUMBER_FIELD;
			static const std::string UNITPRICE_FIELD;
			static const std::string EXCEPTIONPRICE_FIELD;
			static const std::string FINALPRICE_FIELD;
			static const std::string TARE_FIELD;
			static const std::string PROPORTIONALTARE_FIELD;
			static const std::string FINALTARE_FIELD;
			static const std::string BYCOUNT_FIELD;
			static const std::string FINALNETWEIGHT_FIELD;
			static const std::string FIXEDWEIGHT_FIELD;
			static const std::string SHELFLIFEDAYS_FIELD;
			static const std::string SHELFLIFEHOURS_FIELD;
			static const std::string SHELFLIFEMINUTES_FIELD;
			static const std::string PRODUCTLIFEDAYS_FIELD;
			static const std::string PRODUCTLIFEHOURS_FIELD;
			static const std::string PRODUCTLIFEMINUTES_FIELD;
			static const std::string LABEL1_FIELD;
			static const std::string LABEL2_FIELD;
			static const std::string LABEL3_FIELD;
			static const std::string LABEL4_FIELD;
			static const std::string OPERATORNUMBER_FIELD;
			static const std::string OPERATORRFID_FIELD;
			static const std::string OPERATORNAME_FIELD;
			static const std::string OPERATORCHANGETYPE_FIELD;
			static const std::string GRAPHIC1_FIELD;
			static const std::string GRAPHIC2_FIELD;
			static const std::string GRAPHIC3_FIELD;
			static const std::string GRAPHIC4_FIELD;
			static const std::string COOLTEXT_FIELD;
			static const std::string COOLTRACKINGTEXT_FIELD;
			static const std::string CUTTESTNUMBER_FIELD;
			static const std::string PERCENTOFPRIMALWEIGHT_FIELD;
			static const std::string PERCENTOFPRIMALCOST_FIELD;
            static const std::string MODECHANGE_FIELD;

			static const std::string OLDBARCODEDIGIT_FIELD;
			static const std::string OLDUNITPRICE_FIELD;
			static const std::string OLDFINALPRICE_FIELD;
			static const std::string OLDTARE_FIELD;
			static const std::string OLDBYCOUNT_FIELD;
			static const std::string OLDFIXEDWEIGHT_FIELD;
			static const std::string OLDSHELFLIFEDAYS_FIELD;
			static const std::string OLDSHELFLIFEHOURS_FIELD;
			static const std::string OLDSHELFLIFEMINUTES_FIELD;
			static const std::string OLDPRODUCTLIFEDAYS_FIELD;
			static const std::string OLDPRODUCTLIFEHOURS_FIELD;
			static const std::string OLDPRODUCTLIFEMINUTES_FIELD;
			static const std::string OLDLABEL1_FIELD;
			static const std::string OLDLABEL2_FIELD;
			static const std::string OLDLABEL3_FIELD;
			static const std::string OLDLABEL4_FIELD;
			static const std::string OLDGRAPHIC1_FIELD;
			static const std::string OLDGRAPHIC2_FIELD;
			static const std::string OLDGRAPHIC3_FIELD;
			static const std::string OLDGRAPHIC4_FIELD;

			static const std::string SWEETHEARTWEIGHT_FIELD;

			static const std::string::size_type OPERATORNUMBER_SIZE;

		private:

			Util::CSmartPtr<SqliteRecord> rec_;
			Util::CSmartPtr<DBTransactionI> decorated_;

			// Do not allow the default copy constructor.
			SqliteTransaction(const SqliteTransaction& rec);
			// Do not allow the default assignment oper.
			SqliteTransaction operator=(const SqliteTransaction& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITETRANSACTION_H
