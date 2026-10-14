#include "edb.h"
#include DBTRANSACTION_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBTransaction::DBTransaction(edb_adapter::DBTransactionI* d)
		: decorated_(d)
{
}


DBTransaction::DBTransaction(const DBTransaction& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBTransaction::~DBTransaction()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBTransaction& rec)
{
	os << "DBTransaction ScaleID " << rec.getScaleID() << "TransactionID " << rec.getTransactionID() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isTransactionTypeNull() )
		os << ", TransactionType: " << rec.getTransactionType();
	if ( !rec.isTransactionGroupNull() )
		os << ", TransactionGroup: " << rec.getTransactionGroup();
	if ( !rec.isTransactionDateTimeNull() )
		os << ", TransactionDateTime: " << rec.getTransactionDateTime();
	if ( !rec.isTransactionVoidNull() )
		os << ", TransactionVoid: " << rec.getTransactionVoid();
	if ( !rec.isTransactionAffectsProductionNull() )
		os << ", TransactionAffectsProduction: " << rec.getTransactionAffectsProduction();
	if ( !rec.isTransactionReadClearedNull() )
		os << ", TransactionReadCleared: " << rec.getTransactionReadCleared();
	if ( !rec.isTransactionPackagesNull() )
		os << ", TransactionPackages: " << rec.getTransactionPackages();
	if ( !rec.isTransactionUsedSecurityLabelNull() )
		os << ", TransactionUsedSecurityLabel: " << rec.getTransactionUsedSecurityLabel();
	if ( !rec.isTransactionCOOLFlagNull() )
		os << ", TransactionCOOLFlag: " << rec.getTransactionCOOLFlag();
	if ( !rec.isTransactionTotalValueNull() )
		os << ", TransactionTotalValue: " << rec.getTransactionTotalValue();
	if ( !rec.isTransactionOldTotalValueNull() )
		os << ", TransactionOldTotalValue: " << rec.getTransactionOldTotalValue();
	if ( !rec.isDeptNumberNull() )
		os << ", Department: " << rec.getDeptNumber();
	if ( !rec.isProductNumberNull() )
		os << ", ProductNumber: " << rec.getProductNumber();
	if ( !rec.isReportingClassNull() )
		os << ", ReportingClass: " << rec.getReportingClass();
	if ( !rec.isProductTypeNull() )
		os << ", ProductType: " << rec.getProductType();
	if ( !rec.isProductionPlanningTypeNull() )
		os << ", ProductionPlanningType: " << rec.getProductionPlanningType();
	if ( !rec.isProductDescriptionNull() )
		os << ", ProductDescription: " << rec.getProductDescription();
	if ( !rec.isPriceFactorNull() )
		os << ", PriceFactor: " << rec.getPriceFactor();
	if ( !rec.isBarCodePrefixNull() )
		os << ", BarCode Prefix: " << rec.getBarCodePrefix();
	if ( !rec.isBarCodeNumberNull() )
		os << ", BarCode Number: " << rec.getBarCodeNumber();
	if ( !rec.isUnitPriceNull() )
		os << ", UnitPrice: " << rec.getUnitPrice();
	if ( !rec.isExceptionPriceNull() )
		os << ", ExceptionPrice: " << rec.getExceptionPrice();
	if ( !rec.isFinalPriceNull() )
		os << ", FinalPrice: " << rec.getFinalPrice();
	if ( !rec.isTareNull() )
		os << ", Tare: " << rec.getTare();
	if ( !rec.isProportionalTareNull() )
		os << ", ProportionalTare: " << rec.getProportionalTare();
	if ( !rec.isFinalTareNull() )
		os << ", FinalTare: " << rec.getFinalTare();
	if ( !rec.isByCountNull() )
		os << ", ByCount: " << rec.getByCount();
	if ( !rec.isFinalNetWeightNull() )
		os << ", FinalNetWeight: " << rec.getFinalNetWeight();
	if ( !rec.isFixedWeightNull() )
		os << ", FixedWeight: " << rec.getFixedWeight();
	if ( !rec.isShelfLifeDaysNull() )
		os << ", ShelfLifeDays: " << rec.getShelfLifeDays();
	if ( !rec.isShelfLifeHoursNull() )
		os << ", ShelfLifeHours: " << rec.getShelfLifeHours();
	if ( !rec.isShelfLifeMinutesNull() )
		os << ", ShelfLifeMinutes: " << rec.getShelfLifeMinutes();
	if ( !rec.isProductLifeDaysNull() )
		os << ", ProductLifeDays: " << rec.getProductLifeDays();
	if ( !rec.isProductLifeHoursNull() )
		os << ", ProductLifeHours: " << rec.getProductLifeHours();
	if ( !rec.isProductLifeMinutesNull() )
		os << ", ProductLifeMinutes: " << rec.getProductLifeMinutes();
	if ( !rec.isLabel1Null() )
		os << ", Label1: " << rec.getLabel1();
	if ( !rec.isLabel2Null() )
		os << ", Label2: " << rec.getLabel2();
	if ( !rec.isLabel3Null() )
		os << ", Label3: " << rec.getLabel3();
	if ( !rec.isLabel4Null() )
		os << ", Label4: " << rec.getLabel4();
	if ( !rec.isGraphic1Null() )
		os << ", Graphic1: " << rec.getGraphic1();
	if ( !rec.isGraphic2Null() )
		os << ", Graphic2: " << rec.getGraphic2();
	if ( !rec.isGraphic3Null() )
		os << ", Graphic3: " << rec.getGraphic3();
	if ( !rec.isGraphic4Null() )
		os << ", Graphic4: " << rec.getGraphic4();
	if ( !rec.isCoolTextNull() )
		os << ", CoolText: " << rec.getCoolText();
	if ( !rec.isCoolTrackingTextNull() )
		os << ", CoolTrackingText: " << rec.getCoolTrackingText();
	if ( !rec.isOperatorIDNull() )
		os << ", OperatorID: " << rec.getOperatorID();
	if ( !rec.isOperatorNameNull() )
		os << ", OperatorName: " << rec.getOperatorName();
	if ( !rec.isOperatorRFIDNull() )
		os << ", OperatorRFID: " << rec.getOperatorRFID();
	if ( !rec.isOperatorChangeTypeNull() )
		os << ", OperatorChangeType: " << rec.getOperatorChangeType();
	if ( !rec.isCutTestNumberNull() )
		os << ", CutTestNumber: " << rec.getCutTestNumber();
	if ( !rec.isPercentOfPrimalWeightNull() )
		os << ", Percent Of Primal Weight: " << rec.getPercentOfPrimalWeight();
	if ( !rec.isPercentOfPrimalCostNull() )
		os << ", Percent Of Primal Cost: " << rec.getPercentOfPrimalCost();
	if ( !rec.isModeChangeNull() )
		os << ", Mode Change: " << rec.getModeChange();			// 0 = Operate mode, 1 = Supervisor Mode
	if (!rec.isSweetheartWeightNull())
		os << ", Sweetheart Weight: " << rec.getSweetheartWeight();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
