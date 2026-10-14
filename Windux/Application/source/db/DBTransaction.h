#ifndef DBTRANSACTION_H
#define DBTRANSACTION_H


#include <string>


#include "edb.h"
#include DBTRANSACTIONI_INC
#if defined( __linux__ )
    #include DBTRANSACTIONSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBTransaction
	{
			// DBTransactionsRM class is a friend of DBTransaction because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBTransactionsRM;
			friend class DBTransactionTypesRM;
			friend class DBQuery<DBTransaction>;

		public:

			// ProductTypes and subtypes
			typedef edb_adapter::DBTransactionI interface_type;
			typedef DBTransactionsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBTransaction(const DBTransaction& rec);
			~DBTransaction();


			// Methods associated with DBTransactionI
			std::string getScaleID() const { return decorated_->getScaleID(); }
			void setScaleID(const std::string& scaleId) { decorated_->setScaleID(scaleId); }
			bool isScaleIDNull() const { return decorated_->isScaleIDNull(); }
			bool isScaleIDWritable() const { return decorated_->isScaleIDWritable(); }

			std::string getTransactionID() const { return decorated_->getTransactionID(); }
			void setTransactionID(const std::string& id) { decorated_->setTransactionID(id); }
			bool isTransactionIDNull() const { return decorated_->isTransactionIDNull(); }
			bool isTransactionIDWritable() const { return decorated_->isTransactionIDWritable(); }

			int getTransactionType() const { return decorated_->getTransactionType(); }
			void setTransactionType(int type) { decorated_->setTransactionType(type); }
			bool isTransactionTypeNull() const { return decorated_->isTransactionTypeNull(); }
			void setTransactionTypeNull() { decorated_->setTransactionTypeNull(); }
			bool isTransactionTypeWritable() const { return decorated_->isTransactionTypeWritable(); }

			std::string getTransactionGroup() const { return decorated_->getTransactionGroup(); }
			void setTransactionGroup(const std::string& group) { decorated_->setTransactionGroup(group); }
			bool isTransactionGroupNull() const { return decorated_->isTransactionGroupNull(); }
			void setTransactionGroupNull() { decorated_->setTransactionGroupNull(); }
			bool isTransactionGroupWritable() const { return decorated_->isTransactionGroupWritable(); }

			std::string getTransactionDateTime() const { return decorated_->getTransactionDateTime(); }
			void setTransactionDateTime(const std::string& tdt) { decorated_->setTransactionDateTime(tdt); }
			bool isTransactionDateTimeNull() const { return decorated_->isTransactionDateTimeNull(); }
			void setTransactionDateTimeNull() { return decorated_->setTransactionDateTimeNull(); }
			bool isTransactionDateTimeWritable() const { return decorated_->isTransactionDateTimeWritable(); }

			int getTransactionVoid() const { return decorated_->getTransactionVoid(); }
			void setTransactionVoid(int flag) { decorated_->setTransactionVoid(flag); }
			bool isTransactionVoidNull() const { return decorated_->isTransactionVoidNull(); }
			void setTransactionVoidNull() { decorated_->setTransactionVoidNull(); }
			bool isTransactionVoidWritable() const { return decorated_->isTransactionVoidWritable(); }

			int getTransactionAffectsProduction() const { return decorated_->getTransactionAffectsProduction(); }
			void setTransactionAffectsProduction(int flag) { decorated_->setTransactionAffectsProduction(flag); }
			bool isTransactionAffectsProductionNull() const { return decorated_->isTransactionAffectsProductionNull(); }
			void setTransactionAffectsProductionNull() { decorated_->setTransactionAffectsProductionNull(); }
			bool isTransactionAffectsProductionWritable() const { return decorated_->isTransactionAffectsProductionWritable(); }

			int getTransactionReadCleared() const { return decorated_->getTransactionReadCleared(); }
			void setTransactionReadCleared(int flag) { decorated_->setTransactionReadCleared(flag); }
			bool isTransactionReadClearedNull() const { return decorated_->isTransactionReadClearedNull(); }
			void setTransactionReadClearedNull() { decorated_->setTransactionReadClearedNull(); }
			bool isTransactionReadClearedWritable() const { return decorated_->isTransactionReadClearedWritable(); }

			int getTransactionPackages() const { return decorated_->getTransactionPackages(); }
			void setTransactionPackages(int packs) { decorated_->setTransactionPackages(packs); }
			bool isTransactionPackagesNull() const { return decorated_->isTransactionPackagesNull(); }
			void setTransactionPackagesNull() { decorated_->setTransactionPackagesNull(); }
			bool isTransactionPackagesWritable() const { return decorated_->isTransactionPackagesWritable(); }

			bool getTransactionUsedSecurityLabel() const { return decorated_->getTransactionUsedSecurityLabel(); }
			void setTransactionUsedSecurityLabel(bool value) { decorated_->setTransactionUsedSecurityLabel(value); }
			bool isTransactionUsedSecurityLabelNull() const { return decorated_->isTransactionUsedSecurityLabelNull(); }
			void setTransactionUsedSecurityLabelNull() { decorated_->setTransactionUsedSecurityLabelNull(); }
			bool isTransactionUsedSecurityLabelWritable() const { return decorated_->isTransactionUsedSecurityLabelWritable(); }

			bool getTransactionCOOLFlag() const { return decorated_->getTransactionCOOLFlag(); }
			void setTransactionCOOLFlag(bool value) { decorated_->setTransactionCOOLFlag(value); }
			bool isTransactionCOOLFlagNull() const { return decorated_->isTransactionCOOLFlagNull(); }
			void setTransactionCOOLFlagNull() { decorated_->setTransactionCOOLFlagNull(); }
			bool isTransactionCOOLFlagWritable() const { return decorated_->isTransactionCOOLFlagWritable(); }

			int getTransactionTotalValue() const { return decorated_->getTransactionTotalValue(); }
			void setTransactionTotalValue(int value) { decorated_->setTransactionTotalValue(value); }
			bool isTransactionTotalValueNull() const { return decorated_->isTransactionTotalValueNull(); }
			void setTransactionTotalValueNull() { decorated_->setTransactionTotalValueNull(); }
			bool isTransactionTotalValueWritable() const { return decorated_->isTransactionTotalValueWritable(); }

			int getTransactionOldTotalValue() const { return decorated_->getTransactionOldTotalValue(); }
			void setTransactionOldTotalValue(int value) { decorated_->setTransactionOldTotalValue(value); }
			bool isTransactionOldTotalValueNull() const { return decorated_->isTransactionOldTotalValueNull(); }
			void setTransactionOldTotalValueNull() { decorated_->setTransactionOldTotalValueNull(); }
			bool isTransactionOldTotalValueWritable() const { return decorated_->isTransactionOldTotalValueWritable(); }


			std::string getDeptNumber() const { return decorated_->getDeptNumber(); }
			void setDeptNumber(const std::string& number) { decorated_->setDeptNumber(number); }
			bool isDeptNumberNull() const { return decorated_->isDeptNumberNull(); }
			void setDeptNumberNull() { decorated_->setDeptNumberNull(); }
			bool isDeptNumberWritable() const { return decorated_->isDeptNumberWritable(); }

			std::string getProductNumber() const { return decorated_->getProductNumber(); }
			void setProductNumber(const std::string& plu) { decorated_->setProductNumber(plu); }
			bool isProductNumberNull() const { return decorated_->isProductNumberNull(); }
			void setProductNumberNull() { decorated_->setProductNumberNull(); }
			bool isProductNumberWritable() const { return decorated_->isProductNumberWritable(); }

			std::string getReportingClass() const { return decorated_->getReportingClass(); }
			void setReportingClass(const std::string& rc) { decorated_->setReportingClass(rc); }
			bool isReportingClassNull() const { return decorated_->isReportingClassNull(); }
			void setReportingClassNull() { decorated_->setReportingClassNull(); }
			bool isReportingClassWritable() const { return decorated_->isReportingClassWritable(); }

			int getProductType() const { return decorated_->getProductType(); }
			void setProductType(int type) { decorated_->setProductType(type); }
			bool isProductTypeNull() const { return decorated_->isProductTypeNull(); }
			void setProductTypeNull() { decorated_->setProductTypeNull(); }
			bool isProductTypeWritable() const { return decorated_->isProductTypeWritable(); }

			int getProductionPlanningType() const { return decorated_->getProductionPlanningType(); }
			void setProductionPlanningType(int type) { decorated_->setProductionPlanningType(type); }
			bool isProductionPlanningTypeNull() const { return decorated_->isProductionPlanningTypeNull(); }
			void setProductionPlanningTypeNull() { decorated_->setProductionPlanningTypeNull(); }
			bool isProductionPlanningTypeWritable() const { return decorated_->isProductionPlanningTypeWritable(); }

			std::string getProductDescription() const { return decorated_->getProductDescription(); }
			void setProductDescription(const std::string& desc) { decorated_->setProductDescription(desc); }
			bool isProductDescriptionNull() const { return decorated_->isProductDescriptionNull(); }
			void setProductDescriptionNull() { decorated_->setProductDescriptionNull(); }
			bool isProductDescriptionWritable() const { return decorated_->isProductDescriptionWritable(); }

			int getPriceFactor() const { return decorated_->getPriceFactor(); }
			void setPriceFactor(int pf) { decorated_->setPriceFactor(pf); }
			bool isPriceFactorNull() const { return decorated_->isPriceFactorNull(); }
			void setPriceFactorNull() { decorated_->setPriceFactorNull(); }
			bool isPriceFactorWritable() const { return decorated_->isPriceFactorWritable(); }

			int getBarCodePrefix() const { return decorated_->getBarCodePrefix(); }
			void setBarCodePrefix(int bcpr) { decorated_->setBarCodePrefix(bcpr); }
			bool isBarCodePrefixNull() const { return decorated_->isBarCodePrefixNull(); }
			void setBarCodePrefixNull() { decorated_->setBarCodePrefixNull(); }
			bool isBarCodePrefixWritable() const { return decorated_->isBarCodePrefixWritable(); }

			std::string getBarCodeNumber() const { return decorated_->getBarCodeNumber(); }
			void setBarCodeNumber(const std::string& barcode) { decorated_->setBarCodeNumber(barcode); }
			bool isBarCodeNumberNull() const { return decorated_->isBarCodeNumberNull(); }
			void setBarCodeNumberNull() { decorated_->setBarCodeNumberNull(); }
			bool isBarCodeNumberWritable() const { return decorated_->isBarCodeNumberWritable(); }

			int getUnitPrice() const { return decorated_->getUnitPrice(); }
			void setUnitPrice(int price) { decorated_->setUnitPrice(price); }
			bool isUnitPriceNull() const { return decorated_->isUnitPriceNull(); }
			void setUnitPriceNull() { decorated_->setUnitPriceNull(); }
			bool isUnitPriceWritable() const { return decorated_->isUnitPriceWritable(); }

			int getExceptionPrice() const { return decorated_->getExceptionPrice(); }
			void setExceptionPrice(int xp) { decorated_->setExceptionPrice(xp); }
			bool isExceptionPriceNull() const { return decorated_->isExceptionPriceNull(); }
			void setExceptionPriceNull() { decorated_->setExceptionPriceNull(); }
			bool isExceptionPriceWritable() const { return decorated_->isExceptionPriceWritable(); }

			int getFinalPrice() const { return decorated_->getFinalPrice(); }
			void setFinalPrice(int price) { decorated_->setFinalPrice(price); }
			bool isFinalPriceNull() const { return decorated_->isFinalPriceNull(); }
			void setFinalPriceNull() { decorated_->setFinalPriceNull(); }
			bool isFinalPriceWritable() const { return decorated_->isFinalPriceWritable(); }

			int getTare() const { return decorated_->getTare(); }
			void setTare(int t) { decorated_->setTare(t); }
			bool isTareNull() const { return decorated_->isTareNull(); }
			void setTareNull() { return decorated_->setTareNull(); }
			bool isTareWritable() const { return decorated_->isTareWritable(); }

			int getProportionalTare() const { return decorated_->getProportionalTare(); }
			void setProportionalTare(int pt) { decorated_->setProportionalTare(pt); }
			bool isProportionalTareNull() const { return decorated_->isProportionalTareNull(); }
			void setProportionalTareNull() { return decorated_->setProportionalTareNull(); }
			bool isProportionalTareWritable() const { return decorated_->isProportionalTareWritable(); }

			int getFinalTare() const { return decorated_->getFinalTare(); }
			void setFinalTare(int ft) { decorated_->setFinalTare(ft); }
			bool isFinalTareNull() const { return decorated_->isFinalTareNull(); }
			void setFinalTareNull() { return decorated_->setFinalTareNull(); }
			bool isFinalTareWritable() const { return decorated_->isFinalTareWritable(); }

			int getByCount() const { return decorated_->getByCount(); }
			void setByCount(int bc) { decorated_->setByCount(bc); }
			bool isByCountNull() const { return decorated_->isByCountNull(); }
			void setByCountNull() { decorated_->setByCountNull(); }
			bool isByCountWritable() const { return decorated_->isByCountWritable(); }

			int getFinalNetWeight() const { return decorated_->getFinalNetWeight(); }
			void setFinalNetWeight(int nw) { decorated_->setFinalNetWeight(nw); }
			bool isFinalNetWeightNull() const { return decorated_->isFinalNetWeightNull(); }
			void setFinalNetWeightNull() { decorated_->setFinalNetWeightNull(); }
			bool isFinalNetWeightWritable() const { return decorated_->isFinalNetWeightWritable(); }

			int getFixedWeight() const { return decorated_->getFixedWeight(); }
			void setFixedWeight(int nw) { decorated_->setFixedWeight(nw); }
			bool isFixedWeightNull() const { return decorated_->isFixedWeightNull(); }
			void setFixedWeightNull() { decorated_->setFixedWeightNull(); }
			bool isFixedWeightWritable() const { return decorated_->isFixedWeightWritable(); }

			int getShelfLifeDays() const { return decorated_->getShelfLifeDays(); }
			void setShelfLifeDays(int sld) { decorated_->setShelfLifeDays(sld); }
			bool isShelfLifeDaysNull() const { return decorated_->isShelfLifeDaysNull(); }
			void setShelfLifeDaysNull() { decorated_->setShelfLifeDaysNull(); }
			bool isShelfLifeDaysWritable() const { return decorated_->isShelfLifeDaysWritable(); }

			int getShelfLifeHours() const { return decorated_->getShelfLifeHours(); }
			void setShelfLifeHours(int slh) { decorated_->setShelfLifeHours(slh); }
			bool isShelfLifeHoursNull() const { return decorated_->isShelfLifeHoursNull(); }
			void setShelfLifeHoursNull() { decorated_->setShelfLifeHoursNull(); }
			bool isShelfLifeHoursWritable() const { return decorated_->isShelfLifeHoursWritable(); }

			int getShelfLifeMinutes() const { return decorated_->getShelfLifeMinutes(); }
			void setShelfLifeMinutes(int slm) { decorated_->setShelfLifeMinutes(slm); }
			bool isShelfLifeMinutesNull() const { return decorated_->isShelfLifeMinutesNull(); }
			void setShelfLifeMinutesNull() { decorated_->setShelfLifeMinutesNull(); }
			bool isShelfLifeMinutesWritable() const { return decorated_->isShelfLifeMinutesWritable(); }

			int getProductLifeDays() const { return decorated_->getProductLifeDays(); }
			void setProductLifeDays(int pld) { decorated_->setProductLifeDays(pld); }
			bool isProductLifeDaysNull() const { return decorated_->isProductLifeDaysNull(); }
			void setProductLifeDaysNull() { decorated_->setProductLifeDaysNull(); }
			bool isProductLifeDaysWritable() const { return decorated_->isProductLifeDaysWritable(); }

			int getProductLifeHours() const { return decorated_->getProductLifeHours(); }
			void setProductLifeHours(int plh) { decorated_->setProductLifeHours(plh); }
			bool isProductLifeHoursNull() const { return decorated_->isProductLifeHoursNull(); }
			void setProductLifeHoursNull() { decorated_->setProductLifeHoursNull(); }
			bool isProductLifeHoursWritable() const { return decorated_->isProductLifeHoursWritable(); }

			int getProductLifeMinutes() const { return decorated_->getProductLifeMinutes(); }
			void setProductLifeMinutes(int plm) { decorated_->setProductLifeMinutes(plm); }
			bool isProductLifeMinutesNull() const { return decorated_->isProductLifeMinutesNull(); }
			void setProductLifeMinutesNull() { decorated_->setProductLifeMinutesNull(); }
			bool isProductLifeMinutesWritable() const { return decorated_->isProductLifeMinutesWritable(); }

			std::string getLabel1() const { return decorated_->getLabel1(); }
			void setLabel1(const std::string& l1) { decorated_->setLabel1(l1); }
			bool isLabel1Null() const { return decorated_->isLabel1Null(); }
			void setLabel1Null() { decorated_->setLabel1Null(); }
			bool isLabel1Writable() const { return decorated_->isLabel1Writable(); }

			std::string getLabel2() const { return decorated_->getLabel2(); }
			void setLabel2(const std::string& l2) { decorated_->setLabel2(l2); }
			bool isLabel2Null() const { return decorated_->isLabel2Null(); }
			void setLabel2Null() { decorated_->setLabel2Null(); }
			bool isLabel2Writable() const { return decorated_->isLabel2Writable(); }

			std::string getLabel3() const { return decorated_->getLabel3(); }
			void setLabel3(const std::string& l3) { decorated_->setLabel3(l3); }
			bool isLabel3Null() const { return decorated_->isLabel3Null(); }
			void setLabel3Null() { decorated_->setLabel3Null(); }
			bool isLabel3Writable() const { return decorated_->isLabel3Writable(); }

			std::string getLabel4() const { return decorated_->getLabel4(); }
			void setLabel4(const std::string& l4) { decorated_->setLabel4(l4); }
			bool isLabel4Null() const { return decorated_->isLabel4Null(); }
			void setLabel4Null() { decorated_->setLabel4Null(); }
			bool isLabel4Writable() const { return decorated_->isLabel4Writable(); }
			
			std::string getGraphic1() const { return decorated_->getGraphic1(); }
			void setGraphic1(const std::string& g1) { decorated_->setGraphic1(g1); }
			bool isGraphic1Null() const { return decorated_->isGraphic1Null(); }
			void setGraphic1Null() { decorated_->setGraphic1Null(); }
			bool isGraphic1Writable() const { return decorated_->isGraphic1Writable(); }

			std::string getGraphic2() const { return decorated_->getGraphic2(); }
			void setGraphic2(const std::string& g2) { decorated_->setGraphic2(g2); }
			bool isGraphic2Null() const { return decorated_->isGraphic2Null(); }
			void setGraphic2Null() { decorated_->setGraphic2Null(); }
			bool isGraphic2Writable() const { return decorated_->isGraphic2Writable(); }

			std::string getGraphic3() const { return decorated_->getGraphic3(); }
			void setGraphic3(const std::string& g3) { decorated_->setGraphic3(g3); }
			bool isGraphic3Null() const { return decorated_->isGraphic3Null(); }
			void setGraphic3Null() { decorated_->setGraphic3Null(); }
			bool isGraphic3Writable() const { return decorated_->isGraphic3Writable(); }

			std::string getGraphic4() const { return decorated_->getGraphic4(); }
			void setGraphic4(const std::string& g4) { decorated_->setGraphic4(g4); }
			bool isGraphic4Null() const { return decorated_->isGraphic4Null(); }
			void setGraphic4Null() { decorated_->setGraphic4Null(); }
			bool isGraphic4Writable() const { return decorated_->isGraphic4Writable(); }

			std::string getCoolText() const { return decorated_->getCoolText(); }
			void setCoolText(const std::string& text) { decorated_->setCoolText(text); }
			bool isCoolTextNull() const { return decorated_->isCoolTextNull(); }
			void setCoolTextNull() { decorated_->setCoolTextNull(); }
			bool isCoolTextWritable() const { return decorated_->isCoolTextWritable(); }

			std::string getCoolTrackingText() const { return decorated_->getCoolTrackingText(); }
			void setCoolTrackingText(const std::string& text) { decorated_->setCoolTrackingText(text); }
			bool isCoolTrackingTextNull() const { return decorated_->isCoolTrackingTextNull(); }
			void setCoolTrackingTextNull() { decorated_->setCoolTrackingTextNull(); }
			bool isCoolTrackingTextWritable() const { return decorated_->isCoolTrackingTextWritable(); }

			std::string getOperatorID() const { return decorated_->getOperatorID(); }
			void setOperatorID(const std::string& oper) { decorated_->setOperatorID(oper); }
			bool isOperatorIDNull() const { return decorated_->isOperatorIDNull(); }
			void setOperatorIDNull() { decorated_->setOperatorIDNull(); }
			bool isOperatorIDWritable() const { return decorated_->isOperatorIDWritable(); }

			std::string getOperatorName() const { return decorated_->getOperatorName(); }
			void setOperatorName(const std::string& name) { decorated_->setOperatorName(name); }
			bool isOperatorNameNull() const { return decorated_->isOperatorNameNull(); }
			void setOperatorNameNull() { decorated_->isOperatorNameNull(); }
			bool isOperatorNameWritable() const { return decorated_->isOperatorNameWritable(); }

			std::string getOperatorRFID() const { return decorated_->getOperatorRFID(); }
			void setOperatorRFID(const std::string& name) { decorated_->setOperatorRFID(name); }
			bool isOperatorRFIDNull() const { return decorated_->isOperatorRFIDNull(); }
			void setOperatorRFIDNull() { decorated_->isOperatorRFIDNull(); }
			bool isOperatorRFIDWritable() const { return decorated_->isOperatorRFIDWritable(); }

			int getOperatorChangeType() const { return decorated_->getOperatorChangeType(); }
			void setOperatorChangeType(int type) { decorated_->setOperatorChangeType(type); }
			bool isOperatorChangeTypeNull() const { return decorated_->isOperatorChangeTypeNull(); }
			void setOperatorChangeTypeNull() { decorated_->setOperatorChangeTypeNull(); }
			bool isOperatorChangeTypeWritable() const { return decorated_->isOperatorChangeTypeWritable(); }


			int getOldBarCodeDigit() const { return decorated_->getOldBarCodeDigit(); }
			void setOldBarCodeDigit(int bcpr) { decorated_->setOldBarCodeDigit(bcpr); }
			bool isOldBarCodeDigitNull() const { return decorated_->isOldBarCodeDigitNull(); }
			void setOldBarCodeDigitNull() { decorated_->setOldBarCodeDigitNull(); }
			bool isOldBarCodeDigitWritable() const { return decorated_->isOldBarCodeDigitWritable(); }

			int getOldUnitPrice() const { return decorated_->getOldUnitPrice(); }
			void setOldUnitPrice(int price) { decorated_->setOldUnitPrice(price); }
			bool isOldUnitPriceNull() const { return decorated_->isOperatorChangeTypeNull(); }
			void setOldUnitPriceNull() { decorated_->setOldUnitPriceNull(); }
			bool isOldUnitPriceWritable() const { return decorated_->isOldUnitPriceWritable(); }

			int getOldFinalPrice() const { return decorated_->getOldFinalPrice(); }
			void setOldFinalPrice(int price) { decorated_->setOldFinalPrice(price); }
			bool isOldFinalPriceNull() const { return decorated_->isOldFinalPriceNull(); }
			void setOldFinalPriceNull() { decorated_->setOldFinalPriceNull(); }
			bool isOldFinalPriceWritable() const { return decorated_->isOldFinalPriceWritable(); }

			int getOldTare() const { return decorated_->getOldTare(); }
			void setOldTare(int t) { decorated_->setOldTare(t); }
			bool isOldTareNull() const { return decorated_->isOldTareNull(); }
			void setOldTareNull() { decorated_->setOldTareNull(); }
			bool isOldTareWritable() const { return decorated_->isOldTareWritable(); }

			int getOldByCount() const { return decorated_->getOldByCount(); }
			void setOldByCount(int bc) { decorated_->setOldByCount(bc); }
			bool isOldByCountNull() const { return decorated_->isOldByCountNull(); }
			void setOldByCountNull() { decorated_->setOldByCountNull(); }
			bool isOldByCountWritable() const { return decorated_->isOldByCountWritable(); }

			int getOldFixedWeight() const { return decorated_->getOldFixedWeight(); }
			void setOldFixedWeight(int nw) { decorated_->setOldFixedWeight(nw); }
			bool isOldFixedWeightNull() const { return decorated_->isOldFixedWeightNull(); }
			void setOldFixedWeightNull() { decorated_->setOldFixedWeightNull(); }
			bool isOldFixedWeightWritable() const { return decorated_->isOldFixedWeightWritable(); }

			int getOldShelfLifeDays() const { return decorated_->getOldShelfLifeDays(); }
			void setOldShelfLifeDays(int sld) { decorated_->setOldShelfLifeDays(sld); }
			bool isOldShelfLifeDaysNull() const { return decorated_->isOldShelfLifeDaysNull(); }
			void setOldShelfLifeDaysNull() { decorated_->setOldShelfLifeDaysNull(); }
			bool isOldShelfLifeDaysWritable() const { return decorated_->isOldShelfLifeDaysWritable(); }

			int getOldShelfLifeHours() const { return decorated_->getOldShelfLifeHours(); }
			void setOldShelfLifeHours(int slh) { decorated_->setOldShelfLifeHours(slh); }
			bool isOldShelfLifeHoursNull() const { return decorated_->isOldShelfLifeHoursNull(); }
			void setOldShelfLifeHoursNull() { decorated_->setOldShelfLifeHoursNull(); }
			bool isOldShelfLifeHoursWritable() const { return decorated_->isOldShelfLifeHoursWritable(); }

			int getOldShelfLifeMinutes() const { return decorated_->getOldShelfLifeMinutes(); }
			void setOldShelfLifeMinutes(int slm) { decorated_->setOldShelfLifeMinutes(slm); }
			bool isOldShelfLifeMinutesNull() const { return decorated_->isOldShelfLifeMinutesNull(); }
			void setOldShelfLifeMinutesNull() { decorated_->setOldShelfLifeMinutesNull(); }
			bool isOldShelfLifeMinutesWritable() const { return decorated_->isOldShelfLifeMinutesWritable(); }

			int getOldProductLifeDays() const { return decorated_->getOldProductLifeDays(); }
			void setOldProductLifeDays(int pld) { decorated_->setOldProductLifeDays(pld); }
			bool isOldProductLifeDaysNull() const { return decorated_->isOldProductLifeDaysNull(); }
			void setOldProductLifeDaysNull() { decorated_->setOldProductLifeDaysNull(); }
			bool isOldProductLifeDaysWritable() const { return decorated_->isOldProductLifeDaysWritable(); }

			int getOldProductLifeHours() const { return decorated_->getOldProductLifeHours(); }
			void setOldProductLifeHours(int plh) { decorated_->setOldProductLifeHours(plh); }
			bool isOldProductLifeHoursNull() const { return decorated_->isOldProductLifeHoursNull(); }
			void setOldProductLifeHoursNull() { decorated_->setOldProductLifeHoursNull(); }
			bool isOldProductLifeHoursWritable() const { return decorated_->isOldProductLifeHoursWritable(); }

			int getOldProductLifeMinutes() const { return decorated_->getOldProductLifeMinutes(); }
			void setOldProductLifeMinutes(int plm) { decorated_->setOldProductLifeMinutes(plm); }
			bool isOldProductLifeMinutesNull() const { return decorated_->isOldProductLifeMinutesNull(); }
			void setOldProductLifeMinutesNull() { decorated_->setOldProductLifeMinutesNull(); }
			bool isOldProductLifeMinutesWritable() const { return decorated_->isOldProductLifeMinutesWritable(); }

			std::string getOldLabel1() const { return decorated_->getOldLabel1(); }
			void setOldLabel1(const std::string& l1) { decorated_->setOldLabel1(l1); }
			bool isOldLabel1Null() const { return decorated_->isOldLabel1Null(); }
			void setOldLabel1Null() { decorated_->setOldLabel1Null(); }
			bool isOldLabel1Writable() const { return decorated_->isOldLabel1Writable(); }

			std::string getOldLabel2() const { return decorated_->getOldLabel2(); }
			void setOldLabel2(const std::string& l2) { decorated_->setOldLabel2(l2); }
			bool isOldLabel2Null() const { return decorated_->isOldLabel2Null(); }
			void setOldLabel2Null() { decorated_->setOldLabel2Null(); }
			bool isOldLabel2Writable() const { return decorated_->isOldLabel2Writable(); }

			std::string getOldLabel3() const { return decorated_->getOldLabel3(); }
			void setOldLabel3(const std::string& l3) { decorated_->setOldLabel3(l3); }
			bool isOldLabel3Null() const { return decorated_->isOldLabel3Null(); }
			void setOldLabel3Null() { decorated_->setOldLabel3Null(); }
			bool isOldLabel3Writable() const { return decorated_->isOldLabel3Writable(); }

			std::string getOldLabel4() const { return decorated_->getOldLabel4(); }
			void setOldLabel4(const std::string& l4) { decorated_->setOldLabel4(l4); }
			bool isOldLabel4Null() const { return decorated_->isOldLabel4Null(); }
			void setOldLabel4Null() { decorated_->setOldLabel4Null(); }
			bool isOldLabel4Writable() const { return decorated_->isOldLabel4Writable(); }

			std::string getOldGraphic1() const { return decorated_->getOldGraphic1(); }
			void setOldGraphic1(const std::string& g1) { decorated_->setOldGraphic1(g1); }
			bool isOldGraphic1Null() const { return decorated_->isOldGraphic1Null(); }
			void setOldGraphic1Null() { decorated_->setOldGraphic1Null(); }
			bool isOldGraphic1Writable() const { return decorated_->isOldGraphic1Writable(); }

			std::string getOldGraphic2() const { return decorated_->getOldGraphic2(); }
			void setOldGraphic2(const std::string& g2) { decorated_->setOldGraphic2(g2); }
			bool isOldGraphic2Null() const { return decorated_->isOldGraphic2Null(); }
			void setOldGraphic2Null() { decorated_->setOldGraphic2Null(); }
			bool isOldGraphic2Writable() const { return decorated_->isOldGraphic2Writable(); }

			std::string getOldGraphic3() const { return decorated_->getOldGraphic3(); }
			void setOldGraphic3(const std::string& g3) { decorated_->setOldGraphic3(g3); }
			bool isOldGraphic3Null() const { return decorated_->isOldGraphic3Null(); }
			void setOldGraphic3Null() { decorated_->setOldGraphic3Null(); }
			bool isOldGraphic3Writable() const { return decorated_->isOldGraphic3Writable(); }

			std::string getOldGraphic4() const { return decorated_->getOldGraphic4(); }
			void setOldGraphic4(const std::string& g4) { decorated_->setOldGraphic4(g4); }
			bool isOldGraphic4Null() const { return decorated_->isOldGraphic4Null(); }
			void setOldGraphic4Null() { decorated_->setOldGraphic4Null(); }
			bool isOldGraphic4Writable() const { return decorated_->isOldGraphic4Writable(); }

			std::string getCutTestNumber() const { return decorated_->getCutTestNumber(); }
			void setCutTestNumber(const std::string& num) { decorated_->setCutTestNumber(num); }
			bool isCutTestNumberNull() const { return decorated_->isCutTestNumberNull(); }
			void setCutTestNumberNull() { decorated_->setCutTestNumberNull(); }
			bool isCutTestNumberWritable() const { return decorated_->isCutTestNumberWritable(); }

			int getPercentOfPrimalWeight() const { return decorated_->getPercentOfPrimalWeight(); }
			void setPercentOfPrimalWeight(const int pct) { decorated_->setPercentOfPrimalWeight(pct); }
			bool isPercentOfPrimalWeightNull() const { return decorated_->isPercentOfPrimalWeightNull(); }
			void setPercentOfPrimalWeightNull() { decorated_->setPercentOfPrimalWeightNull(); }
			bool isPercentOfPrimalWeightWritable() const { return decorated_->isPercentOfPrimalWeightWritable(); }

			int getPercentOfPrimalCost() const { return decorated_->getPercentOfPrimalCost(); }
			void setPercentOfPrimalCost(const int pct) { decorated_->setPercentOfPrimalCost(pct); }
			bool isPercentOfPrimalCostNull() const { return decorated_->isPercentOfPrimalCostNull(); }
			void setPercentOfPrimalCostNull() { decorated_->setPercentOfPrimalCostNull(); }
			bool isPercentOfPrimalCostWritable() const { return decorated_->isPercentOfPrimalCostWritable(); }

			int getModeChange() const { return decorated_->getModeChange(); }
			void setModeChange(const int mc) { decorated_->setModeChange(mc); }
			bool isModeChangeNull() const { return decorated_->isModeChangeNull(); }
			void setModeChangeNull() { decorated_->setModeChangeNull(); }
			bool isModeChangeWritable() const { return decorated_->isModeChangeWritable(); }

			int getSweetheartWeight() const { return decorated_->getSweetheartWeight(); }
			void setSweetheartWeight(int nw) { decorated_->setSweetheartWeight(nw); }
			bool isSweetheartWeightNull() const { return decorated_->isSweetheartWeightNull(); }
			void setSweetheartWeightNull() { decorated_->setSweetheartWeightNull(); }
			bool isSweetheartWeightWritable() const { return decorated_->isSweetheartWeightWritable(); }

			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getProductNumber(); }
			void setKey(const std::string& key) { decorated_->setProductNumber(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBTransaction& rec);


		private:

			edb_adapter::DBTransactionI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBTransaction(edb_adapter::DBTransactionI* d);

			// Do not allow the default assignment oper.
			DBTransaction operator=(const DBTransaction& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBTRANSACTION_H
