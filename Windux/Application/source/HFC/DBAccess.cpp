/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "DBAccess.h"

#include "DBManager.h"

#include "DBProductsRM.h"
#include "DBProductDefaultsRM.h"
#include "DBClassesRM.h"
#include "DBProductTextsRM.h"
#include "DBProductTextCfgsRM.h"
#include "DBBarcodeScannersRM.h"

#include "DBNutritionsRM.h"

#include "DBOperatorsRM.h"
#include "DBOperatorNotesRM.h"

#include "DBLabelsRM.h"
#include "DBROMLabelsRM.h"
#include "DBLabelTextsRM.h"

#include "DBTransactionsRM.h"
#include "DBTransactionTypesRM.h"

#include "DBConfigurationsRM.h"
#include "DBEmailAlertsRM.h"
#include "DBStatusRM.h"

#include "DBCategoriesRM.h"
#include "DBCategoryStatisticsRM.h"
#include "DBCategoryDetailedStatisticsRM.h"

#include "DBPresentationMethodsRM.h"
#include "DBPresentationMethodStatisticsRM.h"
#include "DBPresentationMethodDetailedStatisticsRM.h"

#include "DBSpecialProgramsRM.h"

#include "DBStreamingGraphicsRM.h"

#include "DBProductStatisticsRM.h"
#include "DBProductDetailedStatisticsRM.h"

#include "DBFlashFilesRM.h"
#include "DBSoftKeysRM.h"
#include "DBSoftKeyFeaturesRM.h"
#include "DBSoftKeyStylesRM.h"

#include "DBCoolCategoriesRM.h"
#include "DBCoolCategoryCountriesRM.h"
#include "DBCoolCategoryPreTextsRM.h"
#include "DBCoolCategoryProductionStepsRM.h"
#include "DBCoolCountriesRM.h"
#include "DBCoolListsRM.h"
#include "DBCoolListTextsRM.h"
#include "DBCoolPreTextsRM.h"
#include "DBCoolProductionStepsRM.h"
#include "DBCoolTextsRM.h"

#include "DBDevicesRM.h"
#include "DBSoftKeyGroupsRM.h"

#include "DBScalesRM.h"
#include "DBBatchesRM.h"
#include "DBSynchronizationChangesRM.h"

#include "DBExpirationLabelsRM.h"
#include "DBKnifeTimeHistoryRM.h"
#include "DBWrapperErrorHistoryRM.h"
#include "DBCuttingTestsRM.h"

#include "DBRecallsRM.h"

#include "DBDeptsRM.h"

#include "util/e_string/utilstring.h"

#include "HobString.h"
#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"


using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int DBAccess::dataType = 0;
std::string DBAccess::dataTypeName("DBAccess"); 
#endif



DBAccess::DBAccess()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}


DBAccess::~DBAccess()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("DBAccess", "DBAccess::~DBAccess()");
	uninitDB();
	Debug::trace("DBAccess", "DBAccess::~DBAccess() done");
}


void DBAccess::initDB()
{
	try
	{
		Debug::trace("DBAccess", "DBAccess::initDB()");

		uninitDB();

		dbMgr = DBManager::getInstance();
		if (dbMgr)
			accessAllowed = true;
		else
		{
			Debug::trace("DBAccess", "DBAccess::initDB() Could not get instance of DBManager");
		}

		Debug::trace("DBAccess", "DBAccess::initDB() done");
	}
	catch (DBException &dbe)
	{
		showDatabaseException("initDB()", dbe);
	}
}


void DBAccess::uninitDB()
{
	Debug::trace("DBAccess", "DBAccess::uninitDB()");

	accessAllowed = false;
	deleteAllRMs();
	if (dbMgr)
	{
		DBManager::destroyInstance(dbMgr);
		dbMgr = (DBManager *)0;
	}

	Debug::trace("DBAccess", "DBAccess::uninitDB() done");
}


DBProductsRM *DBAccess::getDBProductsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBProductsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBProductsRM *)0;
	try
	{
		if (dbProductsRM && !dbProductsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBProductsRM() - deleting disabled dbProductsRM ...");
			delete dbProductsRM;
			dbProductsRM = (DBProductsRM *)0;
		}

		if (!dbProductsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBProductsRM() - creating dbProductsRM ...");
			dbProductsRM = dbMgr->createProducts();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBProductsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBProductsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbProductsRM;
}

DBProductDefaultsRM *DBAccess::getDBProductDefaultsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBProductDefaultsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBProductDefaultsRM *)0;
	try
	{
		if (dbProductDefaultsRM && !dbProductDefaultsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBProductDefaultsRM() - deleting disabled dbProductDefaultsRM ...");
			delete dbProductDefaultsRM;
			dbProductDefaultsRM = (DBProductDefaultsRM *)0;
		}

		if (!dbProductDefaultsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBProductDefaultsRM() - creating dbProductDefaultsRM ...");
			dbProductDefaultsRM = dbMgr->createProductDefaults();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBProductDefaultsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBProductDefaultsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbProductDefaultsRM;
}

DBClassesRM *DBAccess::getDBClassesRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBClassesRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBClassesRM *)0;
	try
	{
		if (dbClassesRM && !dbClassesRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBClassesRM() - deleting disabled dbClassesRM ...");
			delete dbClassesRM;
			dbClassesRM = (DBClassesRM *)0;
		}

		if (!dbClassesRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBClassesRM() - creating dbClassesRM ...");
			dbClassesRM = dbMgr->createClasses();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBClassesRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBClassesRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbClassesRM;
}

DBNutritionsRM *DBAccess::getDBNutritionsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBNutritionsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBNutritionsRM *)0;
	try
	{
		if (dbNutritionsRM && !dbNutritionsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBNutritionsRM() - deleting disabled dbNutritionsRM ...");
			delete dbNutritionsRM;
			dbNutritionsRM = (DBNutritionsRM *)0;
		}

		if (!dbNutritionsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBNutritionsRM() - creating dbNutritionsRM ...");
			dbNutritionsRM = dbMgr->createNutritions();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBNutritionsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBNutritionsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbNutritionsRM;
}


DBProductTextsRM *DBAccess::getDBProductTextsRM(ProdSupportTextType type)
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBProductTextRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed || type <= psttUnknown || type >= psttLastProductText)
		return (DBProductTextsRM *)0;
	try
	{
		if (dbProductTextRMs_[type] && !dbProductTextRMs_[type]->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBProductTextRM() - deleting disabled dbProductTextRM ... type=" + inttostring(type));
			delete dbProductTextRMs_[type];
			dbProductTextRMs_[type] = (DBProductTextsRM *)0;
		}

		if (!dbProductTextRMs_[type])
		{
			Debug::trace("DBAccess", "DBAccess::getDBProductTextRM() - creating dbProductTextRM ... type=" + inttostring(type));
			dbProductTextRMs_[type] = dbMgr->createProductTexts();
			if (dbProductTextRMs_[type])
				dbProductTextRMs_[type]->setType(type);
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBProductTextRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBProductTextRM() took " + util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbProductTextRMs_[type];
}


DBProductTextCfgsRM *DBAccess::getDBProductTextCfgsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBProductTextCfgsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBProductTextCfgsRM *)0;
	try
	{
		if (dbProductTextCfgsRM && !dbProductTextCfgsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBProductTextCfgsRM() - deleting disabled dbProductTextCfgsRM ...");
			delete dbProductTextCfgsRM;
			dbProductTextCfgsRM = (DBProductTextCfgsRM *)0;
		}

		if (!dbProductTextCfgsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBProductTextCfgsRM() - creating dbProductTextCfgsRM ...");
			dbProductTextCfgsRM = dbMgr->createProductTextCfgs();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBProductTextCfgsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBProductTextCfgsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbProductTextCfgsRM;
}

DBBarcodeScannersRM *DBAccess::getDBBarcodeScannersRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBBarcodeScannersRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBBarcodeScannersRM *)0;
	try
	{
		if (dbBarcodeScannersRM && !dbBarcodeScannersRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBBarcodeScannersRM() - deleting disabled dbBarcodeScannersRM ...");
			delete dbBarcodeScannersRM;
			dbBarcodeScannersRM = (DBBarcodeScannersRM *)0;
		}

		if (!dbBarcodeScannersRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBBarcodeScannersRM() - creating dbBarcodeScannersRM ...");
			dbBarcodeScannersRM = dbMgr->createBarcodeScanners();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBBarcodeScannersRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBBarcodeScannersRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbBarcodeScannersRM;
}

DBOperatorsRM *DBAccess::getDBOperatorsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBOperatorsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBOperatorsRM *)0;
	try
	{
		if (dbOperatorsRM && !dbOperatorsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBOperatorsRM() - deleting disabled dbOperatorsRM ...");
			delete dbOperatorsRM;
			dbOperatorsRM = (DBOperatorsRM *)0;
		}

		if (!dbOperatorsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBOperatorsRM() - creating dbOperatorsRM ...");
			dbOperatorsRM = dbMgr->createOperators();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBOperatorsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBOperatorsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbOperatorsRM;
}


DBOperatorNotesRM *DBAccess::getDBOperatorNotesRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBOperatorNotesRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBOperatorNotesRM *)0;
	try
	{
		if (dbOperatorNotesRM && !dbOperatorNotesRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBOperatorNotesRM() - deleting disabled dbOperatorNotesRM ...");
			delete dbOperatorNotesRM;
			dbOperatorNotesRM = (DBOperatorNotesRM *)0;
		}

		if (!dbOperatorNotesRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBOperatorNotesRM() - creating dbOperatorNotesRM ...");
			dbOperatorNotesRM = dbMgr->createOperatorNotes();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBOperatorNotesRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBOperatorNotesRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbOperatorNotesRM;
}


DBLabelsRM *DBAccess::getDBLabelsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBLabelsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBLabelsRM *)0;
	try
	{
		if (dbLabelsRM && !dbLabelsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBLabelsRM() - deleting disabled dbLabelsRM ...");
			delete dbLabelsRM;
			dbLabelsRM = (DBLabelsRM *)0;
		}

		if (!dbLabelsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBLabelsRM() - creating dbLabelsRM ...");
			dbLabelsRM = dbMgr->createLabels();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBLabelsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBLabelsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbLabelsRM;
}

DBROMLabelsRM *DBAccess::getDBROMLabelsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBROMLabelsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBROMLabelsRM *)0;
	try
	{
		if (dbROMLabelsRM && !dbROMLabelsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBROMLabelsRM() - deleting disabled dbROMLabelsRM ...");
			delete dbROMLabelsRM;
			dbROMLabelsRM = (DBROMLabelsRM *)0;
		}

		if (!dbROMLabelsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBROMLabelsRM() - creating dbROMLabelsRM ...");
			dbROMLabelsRM = dbMgr->createROMLabels();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBROMLabelsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBROMLabelsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbROMLabelsRM;
}

DBLabelTextsRM *DBAccess::getDBLabelTextsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBLabelTextsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBLabelTextsRM *)0;
	try
	{
		if (dbLabelTextsRM && !dbLabelTextsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBLabelTextsRM() - deleting disabled dbLabelTextsRM ...");
			delete dbLabelTextsRM;
			dbLabelTextsRM = (DBLabelTextsRM *)0;
		}

		if (!dbLabelTextsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBLabelTextsRM() - creating dbLabelTextsRM ...");
			dbLabelTextsRM = dbMgr->createLabelTexts();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBLabelTextsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBLabelTextsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbLabelTextsRM;
}


DBTransactionsRM *DBAccess::getDBTransactionsRM()
{
	if ( !accessAllowed )
		return 0;

	try
	{
		if ( dbTransactionsRM && !dbTransactionsRM->isEnabled() )
		{
			Debug::trace("DBAccess", "DBAccess::getDBTransactionsRM() - deleting disabled dbTransactionsRM ...");

			delete dbTransactionsRM;
			dbTransactionsRM = 0;
		}

		if ( !dbTransactionsRM )
		{
			Debug::trace("DBAccess", "DBAccess::getDBTransactionsRM() - creating dbTransactionsRM ...");

			dbTransactionsRM = dbMgr->createTransactions();
		}
	}
	catch ( DBException &dbe )
	{
		showDatabaseException("getDBTransactionsRM()", dbe);
	}

	return dbTransactionsRM;
}


DBTransactionTypesRM *DBAccess::getDBTransactionTypesRM()
{
	if ( !accessAllowed )
		return 0;

	try
	{
		if ( dbTransactionTypesRM && !dbTransactionTypesRM->isEnabled() )
		{
			Debug::trace("DBAccess", "DBAccess::getDBTransactionTypesRM() - deleting disabled dbTransactionTypesRM ...");

			delete dbTransactionTypesRM;
			dbTransactionTypesRM = 0;
		}

		if ( !dbTransactionTypesRM )
		{
			Debug::trace("DBAccess", "DBAccess::getDBTransactionTypesRM() - creating dbTransactionTypesRM ...");

			dbTransactionTypesRM = dbMgr->createTransactionTypes();
		}
	}
	catch ( DBException &dbe )
	{
		showDatabaseException("getDBTransactionTypesRM()", dbe);
	}

	return dbTransactionTypesRM;
}


DBConfigurationsRM *DBAccess::getDBConfigurationsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBConfigurationsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBConfigurationsRM *)0;
	try
	{
		if (dbConfigurationsRM && !dbConfigurationsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBConfigurationsRM() - deleting disabled dbConfigurationsRM ...");
			delete dbConfigurationsRM;
			dbConfigurationsRM = (DBConfigurationsRM *)0;
		}

		if (!dbConfigurationsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBConfigurationsRM() - creating dbConfigurationsRM ...");
			dbConfigurationsRM = dbMgr->createConfigurations();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBConfigurationsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBConfigurationsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbConfigurationsRM;
}

DBEmailAlertsRM *DBAccess::getDBEmailAlertsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBEmailAlertsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBEmailAlertsRM *)0;
	try
	{
		if (dbEmailAlertsRM && !dbEmailAlertsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBEmailAlertsRM() - deleting disabled dbEmailAlertsRM ...");
			delete dbEmailAlertsRM;
			dbEmailAlertsRM = (DBEmailAlertsRM *)0;
		}

		if (!dbEmailAlertsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBEmailAlertsRM() - creating dbEmailAlertsRM ...");
			dbEmailAlertsRM = dbMgr->createEmailAlerts();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBEmailAlertsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBEmailAlertsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbEmailAlertsRM;
}

DBStatusRM *DBAccess::getDBStatusRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBStatusRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBStatusRM *)0;
	try
	{
		if (dbStatusRM && !dbStatusRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBStatusRM() - deleting disabled dbStatusRM ...");
			delete dbStatusRM;
			dbStatusRM = (DBStatusRM *)0;
		}

		if (!dbStatusRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBStatusRM() - creating dbStatusRM ...");
			dbStatusRM = dbMgr->createStatus();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBStatusRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBStatusRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbStatusRM;
}


DBCategoriesRM *DBAccess::getDBCategoriesRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBCategoriesRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBCategoriesRM *)0;
	try
	{
		if (dbCategoriesRM && !dbCategoriesRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBCategoriesRM() - deleting disabled dbCategoriesRM ...");
			delete dbCategoriesRM;
			dbCategoriesRM = (DBCategoriesRM *)0;
		}

		if (!dbCategoriesRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBCategoriesRM() - creating dbCategoriesRM ...");
			dbCategoriesRM = dbMgr->createCategories();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBCategoriesRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBCategoriesRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbCategoriesRM;
}


DBCategoryStatisticsRM *DBAccess::getDBCategoryStatisticsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBCategoryStatisticsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBCategoryStatisticsRM *)0;
	try
	{
		if (dbCategoryStatisticsRM && !dbCategoryStatisticsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBCategoryStatisticsRM() - deleting disabled dbCategoryStatisticsRM ...");
			delete dbCategoryStatisticsRM;
			dbCategoryStatisticsRM = (DBCategoryStatisticsRM *)0;
		}

		if (!dbCategoryStatisticsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBCategoryStatisticsRM() - creating dbCategoryStatisticsRM ...");
			dbCategoryStatisticsRM = dbMgr->createCategoryStatistics();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBCategoryStatisticsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBCategoryStatisticsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbCategoryStatisticsRM;
}


DBCategoryDetailedStatisticsRM *DBAccess::getDBCategoryDetailedStatisticsRM()
{
	return 0;
	//DBCategoryDetailedStatisticsRM* createCategoryDetailedStatistics() throw(DBException);
}


DBPresentationMethodsRM *DBAccess::getDBPresentationMethodsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBPresentationMethodsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBPresentationMethodsRM *)0;
	try
	{
		if (dbPresentationMethodsRM && !dbPresentationMethodsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBPresentationMethodsRM() - deleting disabled dbPresentationMethodsRM ...");
			delete dbPresentationMethodsRM;
			dbPresentationMethodsRM = (DBPresentationMethodsRM *)0;
		}

		if (!dbPresentationMethodsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBPresentationMethodsRM() - creating dbPresentationMethodsRM ...");
			dbPresentationMethodsRM = dbMgr->createPresentationMethods();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBPresentationMethodsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBPresentationMethodsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbPresentationMethodsRM;
}


DBPresentationMethodStatisticsRM *DBAccess::getDBPresentationMethodStatisticsRM()
{
	return 0;
	//DBPresentationMethodStatisticsRM* createPresentationMethodStatistics() throw(DBException);
}


DBPresentationMethodDetailedStatisticsRM *DBAccess::getDBPresentationMethodDetailedStatisticsRM()
{
	return 0;
	//DBPresentationMethodDetailedStatisticsRM* createPresentationMethodDetailedStatistics() throw(DBException);
}


DBSpecialProgramsRM *DBAccess::getDBSpecialsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBSpecialsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBSpecialProgramsRM *)0;
	try
	{
		if (dbSpecialsRM && !dbSpecialsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBSpecialsRM() - deleting disabled dbSpecialsRM ...");
			delete dbSpecialsRM;
			dbSpecialsRM = (DBSpecialProgramsRM *)0;
		}

		if (!dbSpecialsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBSpecialsRM() - creating dbSpecialsRM ...");
			dbSpecialsRM = dbMgr->createSpecialPrograms();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBSpecialsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBSpecialsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbSpecialsRM;
}


DBStreamingGraphicsRM *DBAccess::getDBStreamingGraphicsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBStreamingGraphicsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBStreamingGraphicsRM *)0;
	try
	{
		if (dbStreamingGraphicsRM && !dbStreamingGraphicsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBStreamingGraphicsRM() - deleting disabled dbStreamingGraphicsRM ...");
			delete dbStreamingGraphicsRM;
			dbStreamingGraphicsRM = (DBStreamingGraphicsRM *)0;
		}

		if (!dbStreamingGraphicsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBStreamingGraphicsRM() - creating dbStreamingGraphicsRM ...");
			dbStreamingGraphicsRM = dbMgr->createStreamingGraphics();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBStreamingGraphicsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBStreamingGraphicsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbStreamingGraphicsRM;
}


DBProductStatisticsRM *DBAccess::getDBProductStatisticsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBProductStatisticsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBProductStatisticsRM *)0;
	try
	{
		if (dbProductStatisticsRM && !dbProductStatisticsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBProductStatisticsRM() - deleting disabled dbProductStatisticsRM ...");
			delete dbProductStatisticsRM;
			dbProductStatisticsRM = (DBProductStatisticsRM *)0;
		}

		if (!dbProductStatisticsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBProductStatisticsRM() - creating dbProductStatisticsRM ...");
			dbProductStatisticsRM = dbMgr->createProductStatistics();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBProductStatisticsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBProductStatisticsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbProductStatisticsRM;
}


DBProductDetailedStatisticsRM *DBAccess::getDBProductDetailedStatisticsRM()
{
	return 0;
	//DBProductDetailedStatisticsRM* createProductDetailedStatistics() throw(DBException);
}


DBFlashFilesRM *DBAccess::getDBFlashFilesRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBFlashFilesRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBFlashFilesRM *)0;
	try
	{
		if (dbFlashFilesRM && !dbFlashFilesRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBFlashFilesRM() - deleting disabled dbFlashFilesRM ...");
			delete dbFlashFilesRM;
			dbFlashFilesRM = (DBFlashFilesRM *)0;
		}

		if (!dbFlashFilesRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBFlashFilesRM() - creating dbFlashFilesRM ...");
			dbFlashFilesRM = dbMgr->createFlashFiles();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBFlashFilesRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBFlashFilesRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbFlashFilesRM;
}


DBSoftKeysRM *DBAccess::getDBSoftKeysRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBSoftKeysRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBSoftKeysRM *)0;
	try
	{
		if (dbSoftKeysRM && !dbSoftKeysRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBSoftKeysRM() - deleting disabled dbSoftKeysRM ...");
			delete dbSoftKeysRM;
			dbSoftKeysRM = (DBSoftKeysRM *)0;
		}

		if (!dbSoftKeysRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBSoftKeysRM() - creating dbSoftKeysRM ...");
			dbSoftKeysRM = dbMgr->createSoftKeys();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBSoftKeysRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBSoftKeysRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbSoftKeysRM;
}


DBSoftKeyFeaturesRM *DBAccess::getDBSoftKeyFeaturesRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBSoftKeyFeaturesRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBSoftKeyFeaturesRM *)0;
	try
	{
		if (dbSoftKeyFeaturesRM && !dbSoftKeyFeaturesRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBSoftKeyFeaturesRM() - deleting disabled dbSoftKeyFeaturesRM ...");
			delete dbSoftKeyFeaturesRM;
			dbSoftKeyFeaturesRM = (DBSoftKeyFeaturesRM *)0;
		}

		if (!dbSoftKeyFeaturesRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBSoftKeyFeaturesRM() - creating dbSoftKeyFeaturesRM ...");
			dbSoftKeyFeaturesRM = dbMgr->createSoftKeyFeatures();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBSoftKeyFeaturesRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBSoftKeyFeaturesRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbSoftKeyFeaturesRM;
}


DBSoftKeyStylesRM *DBAccess::getDBSoftKeyStylesRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBSoftKeyStylesRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBSoftKeyStylesRM *)0;
	try
	{
		if (dbSoftKeyStylesRM && !dbSoftKeyStylesRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBSoftKeyStylesRM() - deleting disabled dbSoftKeyStylesRM ...");
			delete dbSoftKeyStylesRM;
			dbSoftKeyStylesRM = (DBSoftKeyStylesRM *)0;
		}

		if (!dbSoftKeyStylesRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBSoftKeyStylesRM() - creating dbSoftKeyStylesRM ...");
			dbSoftKeyStylesRM = dbMgr->createSoftKeyStyles();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBSoftKeyStylesRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBSoftKeyStylesRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbSoftKeyStylesRM;
}


DBCoolCategoriesRM *DBAccess::getDBCoolCategoriesRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolCategoriesRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBCoolCategoriesRM *)0;
	try
	{
		if (dbCoolCategoriesRM && !dbCoolCategoriesRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolCategoriesRM() - deleting disabled dbCoolCategoriesRM ...");
			delete dbCoolCategoriesRM;
			dbCoolCategoriesRM = (DBCoolCategoriesRM *)0;
		}

		if (!dbCoolCategoriesRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolCategoriesRM() - creating dbCoolCategoriesRM ...");
			dbCoolCategoriesRM = dbMgr->createCoolCategories();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBCoolCategoriesRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolCategoriesRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbCoolCategoriesRM;
}


DBCoolCategoryCountriesRM *DBAccess::getDBCoolCategoryCountriesRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolCategoryCountriesRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBCoolCategoryCountriesRM *)0;
	try
	{
		if (dbCoolCategoryCountriesRM && !dbCoolCategoryCountriesRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolCategoryCountriesRM() - deleting disabled dbCoolCategoryCountriesRM ...");
			delete dbCoolCategoryCountriesRM;
			dbCoolCategoryCountriesRM = (DBCoolCategoryCountriesRM *)0;
		}

		if (!dbCoolCategoryCountriesRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolCategoryCountriesRM() - creating dbCoolCategoryCountriesRM ...");
			dbCoolCategoryCountriesRM = dbMgr->createCoolCategoryCountries();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBCoolCategoryCountriesRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolCategoryCountriesRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbCoolCategoryCountriesRM;
}


DBCoolCategoryPreTextsRM *DBAccess::getDBCoolCategoryPreTextsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolCategoryPreTextsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBCoolCategoryPreTextsRM *)0;
	try
	{
		if (dbCoolCategoryPreTextsRM && !dbCoolCategoryPreTextsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolCategoryPreTextsRM() - deleting disabled dbCoolCategoryPreTextsRM ...");
			delete dbCoolCategoryPreTextsRM;
			dbCoolCategoryPreTextsRM = (DBCoolCategoryPreTextsRM *)0;
		}

		if (!dbCoolCategoryPreTextsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolCategoryPreTextsRM() - creating dbCoolCategoryPreTextsRM ...");
			dbCoolCategoryPreTextsRM = dbMgr->createCoolCategoryPreTexts();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBCoolCategoryPreTextsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolCategoryPreTextsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbCoolCategoryPreTextsRM;
}


DBCoolCategoryProductionStepsRM *DBAccess::getDBCoolCategoryProductionStepsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolCategoryProductionStepsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBCoolCategoryProductionStepsRM *)0;
	try
	{
		if (dbCoolCategoryProductionStepsRM && !dbCoolCategoryProductionStepsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolCategoryProductionStepsRM() - deleting disabled dbCoolCategoryProductionStepsRM ...");
			delete dbCoolCategoryProductionStepsRM;
			dbCoolCategoryProductionStepsRM = (DBCoolCategoryProductionStepsRM *)0;
		}

		if (!dbCoolCategoryProductionStepsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolCategoryProductionStepsRM() - creating dbCoolCategoryProductionStepsRM ...");
			dbCoolCategoryProductionStepsRM = dbMgr->createCoolCategoryProductionSteps();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBCoolCategoryProductionStepsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolCategoryProductionStepsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbCoolCategoryProductionStepsRM;
}


DBCoolCountriesRM *DBAccess::getDBCoolCountriesRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolCountriesRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBCoolCountriesRM *)0;
	try
	{
		if (dbCoolCountriesRM && !dbCoolCountriesRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolCountriesRM() - deleting disabled dbCoolCountriesRM ...");
			delete dbCoolCountriesRM;
			dbCoolCountriesRM = (DBCoolCountriesRM *)0;
		}

		if (!dbCoolCountriesRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolCountriesRM() - creating dbCoolCountriesRM ...");
			dbCoolCountriesRM = dbMgr->createCoolCountries();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBCoolCountriesRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolCountriesRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbCoolCountriesRM;
}


DBCoolListsRM *DBAccess::getDBCoolListsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolListsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBCoolListsRM *)0;
	try
	{
		if (dbCoolListsRM && !dbCoolListsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolListsRM() - deleting disabled dbCoolListsRM ...");
			delete dbCoolListsRM;
			dbCoolListsRM = (DBCoolListsRM *)0;
		}

		if (!dbCoolListsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolListsRM() - creating dbCoolListsRM ...");
			dbCoolListsRM = dbMgr->createCoolLists();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBCoolListsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolListsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbCoolListsRM;
}


DBCoolListTextsRM *DBAccess::getDBCoolListTextsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolListTextsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBCoolListTextsRM *)0;
	try
	{
		if (dbCoolListTextsRM && !dbCoolListTextsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolListTextsRM() - deleting disabled dbCoolListTextsRM ...");
			delete dbCoolListTextsRM;
			dbCoolListTextsRM = (DBCoolListTextsRM *)0;
		}

		if (!dbCoolListTextsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolListTextsRM() - creating dbCoolListTextsRM ...");
			dbCoolListTextsRM = dbMgr->createCoolListTexts();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBCoolListTextsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolListTextsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbCoolListTextsRM;
}


DBCoolPreTextsRM *DBAccess::getDBCoolPreTextsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolPreTextsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBCoolPreTextsRM *)0;
	try
	{
		if (dbCoolPreTextsRM && !dbCoolPreTextsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolPreTextsRM() - deleting disabled dbCoolPreTextsRM ...");
			delete dbCoolPreTextsRM;
			dbCoolPreTextsRM = (DBCoolPreTextsRM *)0;
		}

		if (!dbCoolPreTextsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolPreTextsRM() - creating dbCoolPreTextsRM ...");
			dbCoolPreTextsRM = dbMgr->createCoolPreTexts();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBCoolPreTextsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolPreTextsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbCoolPreTextsRM;
}


DBCoolProductionStepsRM *DBAccess::getDBCoolProductionStepsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolProductionStepsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBCoolProductionStepsRM *)0;
	try
	{
		if (dbCoolProductionStepsRM && !dbCoolProductionStepsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolProductionStepsRM() - deleting disabled dbCoolProductionStepsRM ...");
			delete dbCoolProductionStepsRM;
			dbCoolProductionStepsRM = (DBCoolProductionStepsRM *)0;
		}

		if (!dbCoolProductionStepsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolProductionStepsRM() - creating dbCoolProductionStepsRM ...");
			dbCoolProductionStepsRM = dbMgr->createCoolProductionSteps();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBCoolProductionStepsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolProductionStepsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbCoolProductionStepsRM;
}


DBCoolTextsRM *DBAccess::getDBCoolTextsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolTextsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBCoolTextsRM *)0;
	try
	{
		if (dbCoolTextsRM && !dbCoolTextsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolTextsRM() - deleting disabled dbCoolTextsRM ...");
			delete dbCoolTextsRM;
			dbCoolTextsRM = (DBCoolTextsRM *)0;
		}

		if (!dbCoolTextsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBCoolTextsRM() - creating dbCoolTextsRM ...");
			dbCoolTextsRM = dbMgr->createCoolTexts();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBCoolTextsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBCoolTextsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbCoolTextsRM;
}


DBDeptsRM *DBAccess::getDBDeptsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBDeptsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBDeptsRM *)0;
	try
	{
		if (dbDeptsRM && !dbDeptsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBDeptsRM() - deleting disabled dbDeptsRM ...");
			delete dbDeptsRM;
			dbDeptsRM = (DBDeptsRM *)0;
		}

		if (!dbDeptsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBDeptsRM() - creating dbDeptsRM ...");
			dbDeptsRM = dbMgr->createDepts();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBDeptsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBDeptsRM() took " +
		util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbDeptsRM;
}


DBDevicesRM *DBAccess::getDBDevicesRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBDevicesRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBDevicesRM *)0;
	try
	{
		if (dbDevicesRM && !dbDevicesRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBDevicesRM() - deleting disabled dbDevicesRM ...");
			delete dbDevicesRM;
			dbDevicesRM = (DBDevicesRM *)0;
		}

		if (!dbDevicesRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBDevicesRM() - creating dbDevicesRM ...");
			dbDevicesRM = dbMgr->createDevices();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBDevicesRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBDevicesRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbDevicesRM;
}


DBSoftKeyGroupsRM *DBAccess::getDBSoftKeyGroupsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBSoftKeyGroupsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBSoftKeyGroupsRM *)0;
	try
	{
		if (dbSoftKeyGroupsRM && !dbSoftKeyGroupsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBSoftKeyGroupsRM() - deleting disabled dbSoftKeyGroupsRM ...");
			delete dbSoftKeyGroupsRM;
			dbSoftKeyGroupsRM = (DBSoftKeyGroupsRM *)0;
		}

		if (!dbSoftKeyGroupsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBSoftKeyGroupsRM() - creating dbSoftKeyGroupsRM ...");
			dbSoftKeyGroupsRM = dbMgr->createSoftKeyGroups();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBSoftKeyGroupsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBSoftKeyGroupsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbSoftKeyGroupsRM;
}


DBScalesRM * DBAccess::getDBScalesRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBScalesRM()");
	util_time::Time t0 = util_time::Time();

	if ( !accessAllowed )
		return 0;

	try
	{
		if ( dbScalesRM && !dbScalesRM->isEnabled() )
		{
			Debug::trace("DBAccess", "DBAccess::getDBScalesRM() - deleting disabled dbScalesRM ...");
			delete dbScalesRM;
			dbScalesRM = 0;
		}

		if ( !dbScalesRM )
		{
			Debug::trace("DBAccess", "DBAccess::getDBScalesRM() - creating dbScalesRM ...");
			dbScalesRM = dbMgr->createScales();
		}
	}
	catch ( DBException &dbe )
	{
		showDatabaseException("getDBScalesRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBScalesRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbScalesRM;
}


DBBatchesRM * DBAccess::getDBBatchesRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBBatchesRM()");
	util_time::Time t0 = util_time::Time();

	if ( !accessAllowed )
		return 0;

	try
	{
		if ( dbBatchesRM && !dbBatchesRM->isEnabled() )
		{
			Debug::trace("DBAccess", "DBAccess::getDBBatchesRM() - deleting disabled dbBatchesRM ...");
			delete dbBatchesRM;
			dbBatchesRM = 0;
		}

		if ( !dbBatchesRM )
		{
			Debug::trace("DBAccess", "DBAccess::getDBBatchesRM() - creating dbBatchesRM ...");
			dbBatchesRM = dbMgr->createBatches();
		}
	}
	catch ( DBException &dbe )
	{
		showDatabaseException("getDBBatchesRM()", dbe);
	}


	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBBatchesRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbBatchesRM;
}


DBSynchronizationChangesRM * DBAccess::getDBSynchronizationChangesRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBSynchronizationChangesRM()");
	util_time::Time t0 = util_time::Time();

	if ( !accessAllowed )
		return 0;

	try
	{
		if ( dbSynchronizationChangesRM && !dbSynchronizationChangesRM->isEnabled() )
		{
			Debug::trace("DBAccess", "DBAccess::getDBSynchronizationChangesRM() - deleting disabled dbSynchronizationChangesRM ...");
			delete dbSynchronizationChangesRM;
			dbSynchronizationChangesRM = 0;
		}

		if ( !dbSynchronizationChangesRM )
		{
			Debug::trace("DBAccess", "DBAccess::getDBSynchronizationChangesRM() - creating dbSynchronizationChangesRM ...");
			dbSynchronizationChangesRM = dbMgr->createSynchronizationChanges();
		}
	}
	catch ( DBException &dbe )
	{
		showDatabaseException("getDBSynchronizationChangesRM()", dbe);
	}


	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBSynchronizationChangesRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbSynchronizationChangesRM;
}


DBExpirationLabelsRM *DBAccess::getDBExpirationLabelsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBExpirationLabelsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBExpirationLabelsRM *)0;
	try
	{
		if (dbExpirationLabelsRM && !dbExpirationLabelsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBExpirationLabelsRM() - deleting disabled dbExpirationLabelsRM ...");
			delete dbExpirationLabelsRM;
			dbExpirationLabelsRM = (DBExpirationLabelsRM *)0;
		}

		if (!dbExpirationLabelsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBExpirationLabelsRM() - creating dbExpirationLabelsRM ...");
			dbExpirationLabelsRM = dbMgr->createExpirationLabels();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBExpirationLabelsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBExpirationLabelsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbExpirationLabelsRM;
}

DBKnifeTimeHistoryRM *DBAccess::getKnifeTimeHistoryRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getKnifeTimeHistoryRM()");
	util_time::Time t0 = util_time::Time();


	if (!accessAllowed)
		return (DBKnifeTimeHistoryRM *)0;
	try
	{
		if (dbKnifeTimeHistoryRM && !dbKnifeTimeHistoryRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getKnifeTimeHistoryRM() - deleting disabled dbKnifeTimeHistoryRM ...");
			delete dbKnifeTimeHistoryRM;
			dbKnifeTimeHistoryRM = (DBKnifeTimeHistoryRM *)0;
		}

		if (!dbKnifeTimeHistoryRM)
		{
			Debug::trace("DBAccess", "DBAccess::getKnifeTimeHistoryRM() - creating dbKnifeTimeHistoryRM ...");
			dbKnifeTimeHistoryRM = dbMgr->createWrapperKnife();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getKnifeTimeHistoryRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getKnifeTimeHistoryRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");


	return dbKnifeTimeHistoryRM;
}

DBWrapperErrorHistoryRM *DBAccess::getWrapperErrorHistoryRM()
{

	Debug::trace("DBAccess.timing", "DBAccess::getWrapperErrorHistoryRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBWrapperErrorHistoryRM *)0;
	try
	{
		if (dbWrapperErrorHistoryRM && !dbWrapperErrorHistoryRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getWrapperErrorHistoryRM() - deleting disabled dbWrapperErrorHistoryRM ...");
			delete dbWrapperErrorHistoryRM;
			dbWrapperErrorHistoryRM = (DBWrapperErrorHistoryRM *)0;
		}

		if (!dbWrapperErrorHistoryRM)
		{
			Debug::trace("DBAccess", "DBAccess::getWrapperErrorHistoryRM() - creating dbWrapperErrorHistoryRM ...");
			dbWrapperErrorHistoryRM = dbMgr->createWrapperErrorHistory();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getWrapperErrorHistoryRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getWrapperErrorHistoryRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbWrapperErrorHistoryRM;
}


DBCuttingTestsRM *DBAccess::getDBCuttingTestsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBCuttingTestsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBCuttingTestsRM *)0;
	try
	{
		if (dbCuttingTestsRM && !dbCuttingTestsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBCuttingTestsRM() - deleting disabled dbCuttingTestsRM ...");
			delete dbCuttingTestsRM;
			dbCuttingTestsRM = (DBCuttingTestsRM *)0;
		}

		if (!dbCuttingTestsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBCuttingTestsRM() - creating dbCuttingTestsRM ...");
			dbCuttingTestsRM = dbMgr->createCuttingTests();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBCuttingTestsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBCuttingTestsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbCuttingTestsRM;
}


DBRecallsRM *DBAccess::getDBRecallsRM()
{
	Debug::trace("DBAccess.timing", "DBAccess::getDBRecallsRM()");
	util_time::Time t0 = util_time::Time();

	if (!accessAllowed)
		return (DBRecallsRM *)0;
	try
	{
		if (dbRecallsRM && !dbRecallsRM->isEnabled())
		{
			Debug::trace("DBAccess", "DBAccess::getDBRecallsRM() - deleting disabled dbRecallsRM ...");
			delete dbRecallsRM;
			dbRecallsRM = (DBRecallsRM *)0;
		}

		if (!dbRecallsRM)
		{
			Debug::trace("DBAccess", "DBAccess::getDBRecallsRM() - creating dbRecallsRM ...");
			dbRecallsRM = dbMgr->createRecalls();
		}
	}
	catch (DBException &dbe)
	{
		showDatabaseException("getDBRecallsRM()", dbe);
	}

	util_time::Time t1 = util_time::Time();
	Debug::trace("DBAccess.timing", "DBAccess::getDBRecallsRM() took " +
			util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

	return dbRecallsRM;
}


//	private
void DBAccess::init()
{
	Debug::trace("DBAccess", "DBAccess::init()");
	accessAllowed = false;
	dbMgr = (DBManager *)0;

	dbProductTextRMs_.resize(psttLastProductText);

	initAllRMs();
	Debug::trace("DBAccess", "DBAccess::init() done");
}

//	private
void DBAccess::initAllRMs()
{
	Debug::trace("DBAccess", "DBAccess::initAllRMs()");

	dbProductsRM = (DBProductsRM *)0;
	dbProductDefaultsRM = (DBProductDefaultsRM *)0;
	dbClassesRM = (DBClassesRM *)0;
	dbNutritionsRM = (DBNutritionsRM *)0;

	if (dbProductTextRMs_.size())
		for (int i=0; i<(int)dbProductTextRMs_.size(); i++)
			dbProductTextRMs_[i] = (DBProductTextsRM *)0;

	dbProductTextCfgsRM = (DBProductTextCfgsRM *)0;
	dbBarcodeScannersRM = (DBBarcodeScannersRM *)0;

	dbOperatorsRM = (DBOperatorsRM *)0;
	dbOperatorNotesRM = (DBOperatorNotesRM *)0;

	dbLabelsRM = (DBLabelsRM *)0;
	dbROMLabelsRM = (DBROMLabelsRM *)0;
	dbLabelTextsRM = (DBLabelTextsRM *)0;

	dbTransactionsRM = (DBTransactionsRM *)0;
	dbTransactionTypesRM = (DBTransactionTypesRM *)0;

	dbConfigurationsRM = (DBConfigurationsRM *)0;
	dbEmailAlertsRM = (DBEmailAlertsRM *)0;
	dbStatusRM = (DBStatusRM *)0;

	dbCategoriesRM = (DBCategoriesRM *)0;
	dbCategoryStatisticsRM = (DBCategoryStatisticsRM *)0;
	dbCategoryDetailedStatisticsRM = (DBCategoryDetailedStatisticsRM *)0;

	dbPresentationMethodsRM = (DBPresentationMethodsRM *)0;
	dbPresentationMethodStatisticsRM = (DBPresentationMethodStatisticsRM *)0;
	dbPresentationMethodDetailedStatisticsRM = (DBPresentationMethodDetailedStatisticsRM *)0;

	dbSpecialsRM = (DBSpecialProgramsRM *)0;

	dbStreamingGraphicsRM = (DBStreamingGraphicsRM *)0;

	dbProductStatisticsRM = (DBProductStatisticsRM *)0;
	dbProductDetailedStatisticsRM = (DBProductDetailedStatisticsRM *)0;

	dbFlashFilesRM = (DBFlashFilesRM *)0;

	dbSoftKeysRM = (DBSoftKeysRM *)0;
	dbSoftKeyFeaturesRM = (DBSoftKeyFeaturesRM *)0;
	dbSoftKeyStylesRM = (DBSoftKeyStylesRM *)0;

	dbCoolCategoriesRM = (DBCoolCategoriesRM *)0;
	dbCoolCategoryCountriesRM = (DBCoolCategoryCountriesRM *)0;
	dbCoolCategoryPreTextsRM = (DBCoolCategoryPreTextsRM *)0;
	dbCoolCategoryProductionStepsRM = (DBCoolCategoryProductionStepsRM *)0;
	dbCoolCountriesRM = (DBCoolCountriesRM *)0;
	dbCoolListsRM = (DBCoolListsRM *)0;
	dbCoolListTextsRM = (DBCoolListTextsRM *)0;
	dbCoolPreTextsRM = (DBCoolPreTextsRM *)0;
	dbCoolProductionStepsRM = (DBCoolProductionStepsRM *)0;
	dbCoolTextsRM = (DBCoolTextsRM *)0;

	dbDevicesRM = (DBDevicesRM *)0;
	dbSoftKeyGroupsRM = (DBSoftKeyGroupsRM *)0;

	dbScalesRM = 0;
	dbBatchesRM = 0;
	dbSynchronizationChangesRM = 0;

	dbExpirationLabelsRM = 0;
	dbKnifeTimeHistoryRM = (DBKnifeTimeHistoryRM *)0;
	dbWrapperErrorHistoryRM = (DBWrapperErrorHistoryRM *)0;
	dbCuttingTestsRM = 0;

	dbRecallsRM = (DBRecallsRM *)0;

	dbDeptsRM = (DBDeptsRM *)0;

	Debug::trace("DBAccess", "DBAccess::initAllRMs() done");
}

//	private
void DBAccess::deleteAllRMs()
{
	try
	{
		Debug::trace("DBAccess", "DBAccess::deleteAllRMs()");

		if (dbProductsRM)
		{
			delete dbProductsRM;
			dbProductsRM = 0;
		}
		if (dbProductDefaultsRM)
		{
			delete dbProductDefaultsRM;
			dbProductDefaultsRM = 0;
		}
		if (dbClassesRM)
		{
			delete dbClassesRM;
			dbClassesRM = 0;
		}
		if (dbNutritionsRM)
		{
			delete dbNutritionsRM;
			dbNutritionsRM = 0;
		}

		if (dbProductTextRMs_.size())
			for (int i=0; i<(int)dbProductTextRMs_.size(); i++)
				if (dbProductTextRMs_[i])
				{
					delete dbProductTextRMs_[i];
					dbProductTextRMs_[i] = 0;
				}

		if (dbProductTextCfgsRM)
		{
			delete dbProductTextCfgsRM;
			dbProductTextCfgsRM = 0;
		}

		if (dbBarcodeScannersRM)
		{
			delete dbBarcodeScannersRM;
			dbBarcodeScannersRM = 0;
		}

		if (dbOperatorsRM)
		{
			delete dbOperatorsRM;
			dbOperatorsRM = 0;
		}
		if (dbOperatorNotesRM)
		{
			delete dbOperatorNotesRM;
			dbOperatorNotesRM = 0;
		}

		if (dbLabelsRM)
		{
			delete dbLabelsRM;
			dbLabelsRM = 0;
		}
		if (dbROMLabelsRM)
		{
			delete dbROMLabelsRM;
			dbROMLabelsRM = 0;
		}
		if (dbLabelTextsRM)
		{
			delete dbLabelTextsRM;
			dbLabelTextsRM = 0;
		}

		if (dbTransactionsRM)
		{
			delete dbTransactionsRM;
			dbTransactionsRM = 0;
		}
		if (dbTransactionTypesRM)
		{
			delete dbTransactionTypesRM;
			dbTransactionTypesRM = 0;
		}

		if (dbConfigurationsRM)
		{
			delete dbConfigurationsRM;
			dbConfigurationsRM = 0;
		}
		if (dbEmailAlertsRM)
		{
			delete dbEmailAlertsRM;
			dbEmailAlertsRM = 0;
		}
		if (dbStatusRM)
		{
			delete dbStatusRM;
			dbStatusRM = 0;
		}

		if (dbCategoriesRM)
		{
			delete dbCategoriesRM;
			dbCategoriesRM = 0;
		}
		if (dbCategoryStatisticsRM)
		{
			delete dbCategoryStatisticsRM;
			dbCategoryStatisticsRM = 0;
		}
		if (dbCategoryDetailedStatisticsRM)
		{
			delete dbCategoryDetailedStatisticsRM;
			dbCategoryDetailedStatisticsRM = 0;
		}

		if (dbPresentationMethodsRM)
		{
			delete dbPresentationMethodsRM;
			dbPresentationMethodsRM = 0;
		}
		if (dbPresentationMethodStatisticsRM)
		{
			delete dbPresentationMethodStatisticsRM;
			dbPresentationMethodStatisticsRM = 0;
		}
		if (dbPresentationMethodDetailedStatisticsRM)
		{
			delete dbPresentationMethodDetailedStatisticsRM;
			dbPresentationMethodDetailedStatisticsRM = 0;
		}

		if (dbSpecialsRM)
		{
			delete dbSpecialsRM;
			dbSpecialsRM = 0;
		}

		if (dbStreamingGraphicsRM)
		{
			delete dbStreamingGraphicsRM;
			dbStreamingGraphicsRM = 0;
		}

		if (dbProductStatisticsRM)
		{
			delete dbProductStatisticsRM;
			dbProductStatisticsRM = 0;
		}
		if (dbProductDetailedStatisticsRM)
		{
			delete dbProductDetailedStatisticsRM;
			dbProductDetailedStatisticsRM = 0;
		}

		if (dbFlashFilesRM)
		{
			delete dbFlashFilesRM;
			dbFlashFilesRM = 0;
		}
		if (dbSoftKeysRM)
		{
			delete dbSoftKeysRM;
			dbSoftKeysRM = 0;
		}
		if (dbSoftKeyFeaturesRM)
		{
			delete dbSoftKeyFeaturesRM;
			dbSoftKeyFeaturesRM = 0;
		}
		if (dbSoftKeyStylesRM)
		{
			delete dbSoftKeyStylesRM;
			dbSoftKeyStylesRM = 0;
		}

		if (dbCoolCategoriesRM)
		{
			delete dbCoolCategoriesRM;
			dbCoolCategoriesRM = 0;
		}
		if (dbCoolCategoryCountriesRM)
		{
			delete dbCoolCategoryCountriesRM;
			dbCoolCategoryCountriesRM = 0;
		}
		if (dbCoolCategoryPreTextsRM)
		{
			delete dbCoolCategoryPreTextsRM;
			dbCoolCategoryPreTextsRM = 0;
		}
		if (dbCoolCategoryProductionStepsRM)
		{
			delete dbCoolCategoryProductionStepsRM;
			dbCoolCategoryProductionStepsRM = 0;
		}
		if (dbCoolCountriesRM)
		{
			delete dbCoolCountriesRM;
			dbCoolCountriesRM = 0;
		}
		if (dbCoolListsRM)
		{
			delete dbCoolListsRM;
			dbCoolListsRM = 0;
		}
		if (dbCoolListTextsRM)
		{
			delete dbCoolListTextsRM;
			dbCoolListTextsRM = 0;
		}
		if (dbCoolPreTextsRM)
		{
			delete dbCoolPreTextsRM;
			dbCoolPreTextsRM = 0;
		}
		if (dbCoolProductionStepsRM)
		{
			delete dbCoolProductionStepsRM;
			dbCoolProductionStepsRM = 0;
		}
		if (dbCoolTextsRM)
		{
			delete dbCoolTextsRM;
			dbCoolTextsRM = 0;
		}

		if (dbDevicesRM)
		{
			delete dbDevicesRM;
			dbDevicesRM = 0;
		}

		if (dbSoftKeyGroupsRM)
		{
			delete dbSoftKeyGroupsRM;
			dbSoftKeyGroupsRM = 0;
		}

		if ( dbScalesRM )
		{
			delete dbScalesRM;
			dbScalesRM = 0;
		}
		if ( dbBatchesRM )
		{
			delete dbBatchesRM;
			dbBatchesRM = 0;
		}
		if ( dbSynchronizationChangesRM )
		{
			delete dbSynchronizationChangesRM;
			dbSynchronizationChangesRM = 0;
		}

		if (dbExpirationLabelsRM)
		{
			delete dbExpirationLabelsRM;
			dbExpirationLabelsRM = 0;
		}

		if (dbKnifeTimeHistoryRM)
		{
			delete dbKnifeTimeHistoryRM;
			dbKnifeTimeHistoryRM = 0;
		}
		if (dbWrapperErrorHistoryRM)
		{
			delete dbWrapperErrorHistoryRM;
			dbWrapperErrorHistoryRM = 0;
		}
		if (dbCuttingTestsRM)
		{
			delete dbCuttingTestsRM;
			dbCuttingTestsRM = 0;
		}

		if (dbRecallsRM)
		{
			delete dbRecallsRM;
			dbRecallsRM = 0;
		}

		if (dbDeptsRM)
		{
			delete dbDeptsRM;
			dbDeptsRM = 0;
		}

		Debug::trace("DBAccess", "DBAccess::deleteAllRMs() done");
	}
	catch (DBException &dbe)
	{
		showDatabaseException("deleteAllRMs()", dbe);
	}
}

void DBAccess::showDatabaseException(const std::string & module, const DBException &dbe)
{
	if (Debug::isTracing("DBAccess.exception"))
	{
		HobString myDebug("DBAccess::");
		myDebug.append(module).append(" ********** database exception **********");
		Debug::trace("DBAccess.exception", myDebug);

		DBException::Type t = dbe.getType();
		myDebug = "   DBException: type: ";
		myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
		Debug::trace("DBAccess.exception", myDebug);
	}
}
