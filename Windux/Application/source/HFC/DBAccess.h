/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef DBACCESS_H
#define DBACCESS_H

#include "evolution.h"
#include "DBException.h"	// case sensitive in Linux
#include "Product.h"

using namespace edb; 


namespace edb 
{
class DBManager;
class DBProductsRM;
class DBProductDefaultsRM;
class DBClassesRM;
class DBProductTextsRM;
class DBProductTextCfgsRM;
class DBBarcodeScannersRM;
class DBNutritionsRM;

class DBOperatorsRM;
class DBOperatorNotesRM;

class DBLabelsRM;
class DBROMLabelsRM;
class DBLabelTextsRM;

class DBTransactionsRM;
class DBTransactionTypesRM;

class DBConfigurationsRM;
class DBEmailAlertsRM;
class DBStatusRM;

class DBCategoriesRM;
class DBCategoryStatisticsRM;
class DBCategoryDetailedStatisticsRM;

class DBPresentationMethodsRM;
class DBPresentationMethodStatisticsRM;
class DBPresentationMethodDetailedStatisticsRM;

class DBSpecialProgramsRM;

class DBStreamingGraphicsRM;

class DBProductStatisticsRM;
class DBProductDetailedStatisticsRM;

class DBFlashFilesRM;
class DBSoftKeysRM;
class DBSoftKeyFeaturesRM;
class DBSoftKeyStylesRM;
class DBCoolCategoriesRM;
class DBCoolCountriesRM;
class DBCoolCategoryCountriesRM;
class DBCoolCategoryPreTextsRM;
class DBCoolCategoryProductionStepsRM;

class DBCoolListsRM;
class DBCoolListTextsRM;
class DBCoolPreTextsRM;
class DBCoolProductionStepsRM;
class DBCoolTextsRM;

class DBDeptsRM;
class DBDevicesRM;
class DBSoftKeyGroupsRM;

class DBScalesRM;
class DBBatchesRM;
class DBSynchronizationChangesRM;

class DBExpirationLabelsRM;
class DBKnifeTimeHistoryRM;
class DBWrapperErrorHistoryRM;
class DBCuttingTestsRM;

class DBRecallsRM;
}//end of namespace


class DBAccess
{
	public:
		DBAccess();
		virtual ~DBAccess();

		void initDB();

		DBProductsRM *getDBProductsRM();
		DBProductDefaultsRM *getDBProductDefaultsRM();
		DBClassesRM *getDBClassesRM();
		DBNutritionsRM *getDBNutritionsRM();

		DBProductTextsRM *getDBProductTextsRM(ProdSupportTextType type);
		DBProductTextCfgsRM *getDBProductTextCfgsRM();
		DBBarcodeScannersRM *getDBBarcodeScannersRM();

		DBOperatorsRM *getDBOperatorsRM();
		DBOperatorNotesRM *getDBOperatorNotesRM();

		DBLabelsRM *getDBLabelsRM();
		DBROMLabelsRM *getDBROMLabelsRM();
		DBLabelTextsRM *getDBLabelTextsRM();

		DBTransactionsRM *getDBTransactionsRM();
		DBTransactionTypesRM *getDBTransactionTypesRM();

		DBConfigurationsRM *getDBConfigurationsRM();
		DBEmailAlertsRM *getDBEmailAlertsRM();
		DBStatusRM *getDBStatusRM();

		DBCategoriesRM *getDBCategoriesRM();
		DBCategoryStatisticsRM *getDBCategoryStatisticsRM();
		DBCategoryDetailedStatisticsRM *getDBCategoryDetailedStatisticsRM();

		DBPresentationMethodsRM *getDBPresentationMethodsRM();
		DBPresentationMethodStatisticsRM *getDBPresentationMethodStatisticsRM();
		DBPresentationMethodDetailedStatisticsRM *getDBPresentationMethodDetailedStatisticsRM();

		DBSpecialProgramsRM *getDBSpecialsRM();

		DBStreamingGraphicsRM *getDBStreamingGraphicsRM();

		DBProductStatisticsRM *getDBProductStatisticsRM();
		DBProductDetailedStatisticsRM *getDBProductDetailedStatisticsRM();

		DBFlashFilesRM *getDBFlashFilesRM();
		DBSoftKeysRM *getDBSoftKeysRM();
		DBSoftKeyFeaturesRM *getDBSoftKeyFeaturesRM();
		DBSoftKeyStylesRM *getDBSoftKeyStylesRM();

		DBCoolCategoriesRM *getDBCoolCategoriesRM();
		DBCoolCategoryCountriesRM *getDBCoolCategoryCountriesRM();
		DBCoolCategoryPreTextsRM *getDBCoolCategoryPreTextsRM();
		DBCoolCategoryProductionStepsRM *getDBCoolCategoryProductionStepsRM();
		DBCoolCountriesRM *getDBCoolCountriesRM();
		DBCoolListsRM *getDBCoolListsRM();
		DBCoolListTextsRM *getDBCoolListTextsRM();
		DBCoolPreTextsRM *getDBCoolPreTextsRM();
		DBCoolProductionStepsRM *getDBCoolProductionStepsRM();
		DBCoolTextsRM *getDBCoolTextsRM();

		DBDeptsRM *getDBDeptsRM();
		DBDevicesRM *getDBDevicesRM();
		DBSoftKeyGroupsRM *getDBSoftKeyGroupsRM();

		DBScalesRM * getDBScalesRM();
		DBBatchesRM * getDBBatchesRM();
		DBSynchronizationChangesRM * getDBSynchronizationChangesRM();

		DBExpirationLabelsRM *getDBExpirationLabelsRM();

		DBKnifeTimeHistoryRM *getKnifeTimeHistoryRM();
		DBWrapperErrorHistoryRM *getWrapperErrorHistoryRM();
		DBCuttingTestsRM *getDBCuttingTestsRM();

		DBRecallsRM *getDBRecallsRM();


	protected:

		edb::DBManager *dbMgr;

		void uninitDB();
		void showDatabaseException(const std::string & module, const DBException &dbe);

	private:

		bool accessAllowed;

		edb::DBProductsRM *dbProductsRM;
		edb::DBProductDefaultsRM *dbProductDefaultsRM;
		edb::DBClassesRM *dbClassesRM;
		edb::DBNutritionsRM *dbNutritionsRM;

		std::vector<edb::DBProductTextsRM *> dbProductTextRMs_;
		edb::DBProductTextCfgsRM *dbProductTextCfgsRM;
		edb::DBBarcodeScannersRM *dbBarcodeScannersRM;

		edb::DBOperatorsRM *dbOperatorsRM;
		edb::DBOperatorNotesRM *dbOperatorNotesRM;

		edb::DBLabelsRM *dbLabelsRM;
		edb::DBROMLabelsRM *dbROMLabelsRM;
		edb::DBLabelTextsRM *dbLabelTextsRM;

		edb::DBTransactionsRM *dbTransactionsRM;
		edb::DBTransactionTypesRM *dbTransactionTypesRM;

		edb::DBConfigurationsRM *dbConfigurationsRM;
		edb::DBEmailAlertsRM *dbEmailAlertsRM;
		edb::DBStatusRM *dbStatusRM;

		edb::DBCategoriesRM *dbCategoriesRM;
		edb::DBCategoryStatisticsRM *dbCategoryStatisticsRM;
		edb::DBCategoryDetailedStatisticsRM *dbCategoryDetailedStatisticsRM;

		edb::DBPresentationMethodsRM *dbPresentationMethodsRM;
		edb::DBPresentationMethodStatisticsRM *dbPresentationMethodStatisticsRM;
		edb::DBPresentationMethodDetailedStatisticsRM *dbPresentationMethodDetailedStatisticsRM;

		edb::DBSpecialProgramsRM *dbSpecialsRM;

		edb::DBStreamingGraphicsRM *dbStreamingGraphicsRM;

		edb::DBProductStatisticsRM *dbProductStatisticsRM;
		edb::DBProductDetailedStatisticsRM *dbProductDetailedStatisticsRM;

		edb::DBFlashFilesRM *dbFlashFilesRM;
		edb::DBSoftKeysRM *dbSoftKeysRM;
		edb::DBSoftKeyFeaturesRM *dbSoftKeyFeaturesRM;
		edb::DBSoftKeyStylesRM *dbSoftKeyStylesRM;

		edb::DBCoolCategoriesRM *dbCoolCategoriesRM;
		edb::DBCoolCategoryCountriesRM *dbCoolCategoryCountriesRM;
		edb::DBCoolCategoryPreTextsRM *dbCoolCategoryPreTextsRM;
		edb::DBCoolCategoryProductionStepsRM *dbCoolCategoryProductionStepsRM;
		edb::DBCoolCountriesRM *dbCoolCountriesRM;
		edb::DBCoolListsRM *dbCoolListsRM;
		edb::DBCoolListTextsRM *dbCoolListTextsRM;
		edb::DBCoolPreTextsRM *dbCoolPreTextsRM;
		edb::DBCoolProductionStepsRM *dbCoolProductionStepsRM;
		edb::DBCoolTextsRM *dbCoolTextsRM;

		edb::DBDeptsRM *dbDeptsRM;
		edb::DBDevicesRM *dbDevicesRM;
		edb::DBSoftKeyGroupsRM *dbSoftKeyGroupsRM;

		edb::DBScalesRM * dbScalesRM;
		edb::DBBatchesRM * dbBatchesRM;
		edb::DBSynchronizationChangesRM * dbSynchronizationChangesRM;

		edb::DBExpirationLabelsRM *dbExpirationLabelsRM;

		edb::DBKnifeTimeHistoryRM *dbKnifeTimeHistoryRM;
		edb::DBWrapperErrorHistoryRM *dbWrapperErrorHistoryRM;
		edb::DBCuttingTestsRM *dbCuttingTestsRM;

		edb::DBRecallsRM *dbRecallsRM;

		void init();
		void initAllRMs();
		void deleteAllRMs();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //DBACCESS_H




