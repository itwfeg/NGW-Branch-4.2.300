#include "edb.h"
#include DBFACTORY_INC
#include DBMANAGER_INC

#include DBBATCHESRM_INC
#include DBBARCODESCANNERSRM_INC
#include DBCATEGORIESRM_INC
#include DBCATEGORYDETAILEDSTATISTICSRM_INC
#include DBCATEGORYSTATISTICSRM_INC
#include DBCLASSESRM_INC
#include DBCONFIGURATIONSRM_INC
#include DBCOOLCATEGORIESRM_INC
#include DBCOOLCATEGORYCOUNTRIESRM_INC
#include DBCOOLCATEGORYPRETEXTSRM_INC
#include DBCOOLCATEGORYPRODUCTIONSTEPSRM_INC
#include DBCOOLCOUNTRIESRM_INC
#include DBCOOLLISTSRM_INC
#include DBCOOLLISTTEXTSRM_INC
#include DBCOOLPRETEXTSRM_INC
#include DBCOOLPRODUCTIONSTEPSRM_INC
#include DBCOOLTEXTSRM_INC
#include DBCUTTINGTESTSRM_INC
#include DBDEPTSRM_INC
#include DBDEVICESRM_INC
#include DBEMAILALERTSRM_INC
#include DBEXPIRATIONLABELSRM_INC
#include DBFLASHFILESRM_INC
#include DBKNIFETIMEHISTORYRM_INC
#include DBLABELSRM_INC
#include DBLABELTEXTSRM_INC
#include DBNETWORKERRORSRM_INC
#include DBNUTRITIONSRM_INC
#include DBOPERATORSRM_INC
#include DBOPERATORNOTESRM_INC
#include DBPRESENTATIONMETHODSRM_INC
#include DBPRESENTATIONMETHODDETAILEDSTATISTICSRM_INC
#include DBPRESENTATIONMETHODSTATISTICSRM_INC
#include DBPRODUCTSRM_INC
#include DBPRODUCTDEFAULTSRM_INC
#include DBPRODUCTDETAILEDSTATISTICSRM_INC
#include DBPRODUCTSTATISTICSRM_INC
#include DBPRODUCTTEXTSRM_INC
#include DBPRODUCTTEXTCFGSRM_INC
#include DBRECALLSRM_INC
#include DBROMLABELSRM_INC
#include DBSCALESRM_INC
#include DBSOFTKEYSRM_INC
#include DBSOFTKEYFEATURESRM_INC
#include DBSOFTKEYGROUPSRM_INC
#include DBSOFTKEYSTYLESRM_INC
#include DBSPECIALPROGRAMSRM_INC
#include DBSTATUSRM_INC
#include DBSTREAMINGGRAPHICSRM_INC
#include DBSYNCHRONIZATIONCHANGESRM_INC
#include DBTRANSACTIONSRM_INC
#include DBTRANSACTIONTYPESRM_INC
#include DBWRAPPERERRORHISTORYRM_INC


#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



DBFactory::DBFactory(DBManager* dbm, edb_adapter::DBFactoryI* f) : dbm_(dbm), decorated_(f)
{
}


DBFactory::~DBFactory()
{
	if ( decorated_ )
		delete decorated_;
}


// Template Factory Method
template <> DBBatchesRM* DBFactory::create<DBBatchesRM>()
{
	return createBatches();
}

template <> DBBarcodeScannersRM* DBFactory::create<DBBarcodeScannersRM>()
{
	return createBarcodeScanners();
}

template <> DBCategoriesRM* DBFactory::create<DBCategoriesRM>()
{
	return createCategories();
}


template <> DBCategoryDetailedStatisticsRM* DBFactory::create<DBCategoryDetailedStatisticsRM>()
{
	return createCategoryDetailedStatistics();
}


template <> DBCategoryStatisticsRM* DBFactory::create<DBCategoryStatisticsRM>()
{
	return createCategoryStatistics();
}


template <> DBClassesRM* DBFactory::create<DBClassesRM>()
{
	return createClasses();
}


template <> DBConfigurationsRM* DBFactory::create<DBConfigurationsRM>()
{
	return createConfigurations();
}


template <> DBCoolCategoriesRM* DBFactory::create<DBCoolCategoriesRM>()
{
	return createCoolCategories();
}


template <> DBCoolCategoryCountriesRM* DBFactory::create<DBCoolCategoryCountriesRM>()
{
	return createCoolCategoryCountries();
}


template <> DBCoolCategoryPreTextsRM* DBFactory::create<DBCoolCategoryPreTextsRM>()
{
	return createCoolCategoryPreTexts();
}


template <> DBCoolCategoryProductionStepsRM* DBFactory::create<DBCoolCategoryProductionStepsRM>()
{
	return createCoolCategoryProductionSteps();
}


template <> DBCoolCountriesRM* DBFactory::create<DBCoolCountriesRM>()
{
	return createCoolCountries();
}


template <> DBCoolListsRM* DBFactory::create<DBCoolListsRM>()
{
	return createCoolLists();
}


template <> DBCoolListTextsRM* DBFactory::create<DBCoolListTextsRM>()
{
	return createCoolListTexts();
}


template <> DBCoolPreTextsRM* DBFactory::create<DBCoolPreTextsRM>()
{
	return createCoolPreTexts();
}


template <> DBCoolProductionStepsRM* DBFactory::create<DBCoolProductionStepsRM>()
{
	return createCoolProductionSteps();
}


template <> DBCoolTextsRM* DBFactory::create<DBCoolTextsRM>()
{
	return createCoolTexts();
}


template <> DBCuttingTestsRM* DBFactory::create<DBCuttingTestsRM>()
{
	return createCuttingTests();
}


template <> DBDeptsRM* DBFactory::create<DBDeptsRM>()
{
	return createDepts();
}


template <> DBDevicesRM* DBFactory::create<DBDevicesRM>()
{
	return createDevices();
}


template <> DBEmailAlertsRM* DBFactory::create<DBEmailAlertsRM>()
{
	return createEmailAlerts();
}


template <> DBExpirationLabelsRM* DBFactory::create<DBExpirationLabelsRM>()
{
	return createExpirationLabels();
}


template <> DBFlashFilesRM* DBFactory::create<DBFlashFilesRM>()
{
	return createFlashFiles();
}


template <> DBLabelsRM* DBFactory::create<DBLabelsRM>()
{
	return createLabels();
}


template <> DBLabelTextsRM* DBFactory::create<DBLabelTextsRM>()
{
	return createLabelTexts();
}


template <> DBNetworkErrorsRM* DBFactory::create<DBNetworkErrorsRM>()
{
	return createNetworkErrors();
}


template <> DBNutritionsRM* DBFactory::create<DBNutritionsRM>()
{
	return createNutritions();
}


template <> DBOperatorsRM* DBFactory::create<DBOperatorsRM>()
{
	return createOperators();
}


template <> DBOperatorNotesRM* DBFactory::create<DBOperatorNotesRM>()
{
	return createOperatorNotes();
}


template <> DBPresentationMethodsRM* DBFactory::create<DBPresentationMethodsRM>()
{
	return createPresentationMethods();
}


template <> DBPresentationMethodDetailedStatisticsRM* DBFactory::create<DBPresentationMethodDetailedStatisticsRM>()
{
	return createPresentationMethodDetailedStatistics();
}


template <> DBPresentationMethodStatisticsRM* DBFactory::create<DBPresentationMethodStatisticsRM>()
{
	return createPresentationMethodStatistics();
}


template <> DBProductsRM* DBFactory::create<DBProductsRM>()
{
	return createProducts();
}


template <> DBProductDefaultsRM* DBFactory::create<DBProductDefaultsRM>()
{
	return createProductDefaults();
}


template <> DBProductDetailedStatisticsRM* DBFactory::create<DBProductDetailedStatisticsRM>()
{
	return createProductDetailedStatistics();
}


template <> DBProductStatisticsRM* DBFactory::create<DBProductStatisticsRM>()
{
	return createProductStatistics();
}


template <> DBProductTextsRM* DBFactory::create<DBProductTextsRM>()
{
	return createProductTexts();
}


template <> DBProductTextCfgsRM* DBFactory::create<DBProductTextCfgsRM>()
{
	return createProductTextCfgs();
}


template <> DBRecallsRM* DBFactory::create<DBRecallsRM>()
{
	return createRecalls();
}


template <> DBROMLabelsRM* DBFactory::create<DBROMLabelsRM>()
{
	return createROMLabels();
}


template <> DBScalesRM* DBFactory::create<DBScalesRM>()
{
	return createScales();
}


template <> DBSoftKeysRM* DBFactory::create<DBSoftKeysRM>()
{
	return createSoftKeys();
}


template <> DBSoftKeyFeaturesRM* DBFactory::create<DBSoftKeyFeaturesRM>()
{
	return createSoftKeyFeatures();
}


template <> DBSoftKeyGroupsRM* DBFactory::create<DBSoftKeyGroupsRM>()
{
	return createSoftKeyGroups();
}


template <> DBSoftKeyStylesRM* DBFactory::create<DBSoftKeyStylesRM>()
{
	return createSoftKeyStyles();
}


template <> DBSpecialProgramsRM* DBFactory::create<DBSpecialProgramsRM>()
{
	return createSpecialPrograms();
}


template <> DBStatusRM* DBFactory::create<DBStatusRM>()
{
	return createStatus();
}


template <> DBStreamingGraphicsRM* DBFactory::create<DBStreamingGraphicsRM>()
{
	return createStreamingGraphics();
}


template <> DBSynchronizationChangesRM* DBFactory::create<DBSynchronizationChangesRM>()
{
	return createSynchronizationChanges();
}


template <> DBTransactionsRM* DBFactory::create<DBTransactionsRM>()
{
	return createTransactions();
}


template <> DBTransactionTypesRM* DBFactory::create<DBTransactionTypesRM>()
{
	return createTransactionTypes();
}


template <> DBKnifeTimeHistoryRM* DBFactory::create<DBKnifeTimeHistoryRM>()
{
	return createWrapperKnife();
}


template <> DBWrapperErrorHistoryRM* DBFactory::create<DBWrapperErrorHistoryRM>()
{
	return createWrapperErrorHistory();
}


// Factory Methods
DBBatchesRM* DBFactory::createBatches()
{
	DBBatchesRM* rm = new DBBatchesRM(dbm_, DBRecordManager::Batches, decorated_->createBatches());

	Debug::trace("DBFactory.create", "Created DBBatchesRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}

DBBarcodeScannersRM* DBFactory::createBarcodeScanners()
{
	DBBarcodeScannersRM* rm = new DBBarcodeScannersRM(dbm_, DBRecordManager::BarcodeScanners, decorated_->createBarcodeScanners());

	Debug::trace("DBFactory.create", "Created DBBarcodeScannersRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}

DBCategoriesRM* DBFactory::createCategories()
{
	DBCategoriesRM* rm = new DBCategoriesRM(dbm_, DBRecordManager::Categories, decorated_->createCategories());

	Debug::trace("DBFactory.create", "Created DBCategoriesRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBCategoryDetailedStatisticsRM* DBFactory::createCategoryDetailedStatistics()
{
	DBCategoryDetailedStatisticsRM* rm = new DBCategoryDetailedStatisticsRM(dbm_, DBRecordManager::CategoryDetailedStatistics, decorated_->createCategoryDetailedStatistics());

	Debug::trace("DBFactory.create", "Created DBCategoryDetailedStatisticsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBCategoryStatisticsRM* DBFactory::createCategoryStatistics()
{
	DBCategoryStatisticsRM* rm = new DBCategoryStatisticsRM(dbm_, DBRecordManager::CategoryStatistics, decorated_->createCategoryStatistics());

	Debug::trace("DBFactory.create", "Created DBCategoryStatisticsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBClassesRM* DBFactory::createClasses()
{
	DBClassesRM* rm = new DBClassesRM(dbm_, DBRecordManager::Classes, decorated_->createClasses());

	Debug::trace("DBFactory.create", "Created DBClassesRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBConfigurationsRM* DBFactory::createConfigurations()
{
	DBConfigurationsRM* rm = new DBConfigurationsRM(dbm_, DBRecordManager::Configurations, decorated_->createConfigurations());

	Debug::trace("DBFactory.create", "Created DBConfigurationsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBCoolCategoriesRM* DBFactory::createCoolCategories()
{
	DBCoolCategoriesRM* rm = new DBCoolCategoriesRM(dbm_, DBRecordManager::CoolCategories, decorated_->createCoolCategories());

	Debug::trace("DBFactory.create", "Created DBCoolCategoriesRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBCoolCategoryCountriesRM* DBFactory::createCoolCategoryCountries()
{
	DBCoolCategoryCountriesRM* rm = new DBCoolCategoryCountriesRM(dbm_, DBRecordManager::CoolCategoryCountries, decorated_->createCoolCategoryCountries());

	Debug::trace("DBFactory.create", "Created DBCoolCategoryCountriesRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBCoolCategoryPreTextsRM* DBFactory::createCoolCategoryPreTexts()
{
	DBCoolCategoryPreTextsRM* rm = new DBCoolCategoryPreTextsRM(dbm_, DBRecordManager::CoolCategoryPreTexts, decorated_->createCoolCategoryPreTexts());

	Debug::trace("DBFactory.create", "Created DBCoolCategoryPreTextsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBCoolCategoryProductionStepsRM* DBFactory::createCoolCategoryProductionSteps()
{
	DBCoolCategoryProductionStepsRM* rm = new DBCoolCategoryProductionStepsRM(dbm_, DBRecordManager::CoolCategoryProductionSteps, decorated_->createCoolCategoryProductionSteps());

	Debug::trace("DBFactory.create", "Created DBCoolCategoryProductionStepsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBCoolCountriesRM* DBFactory::createCoolCountries()
{
	DBCoolCountriesRM* rm = new DBCoolCountriesRM(dbm_, DBRecordManager::CoolCountries, decorated_->createCoolCountries());

	Debug::trace("DBFactory.create", "Created DBCoolCountriesRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBCoolListsRM* DBFactory::createCoolLists()
{
	DBCoolListsRM* rm = new DBCoolListsRM(dbm_, DBRecordManager::CoolLists, decorated_->createCoolLists());

	Debug::trace("DBFactory.create", "Created DBCoolListsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBCoolListTextsRM* DBFactory::createCoolListTexts()
{
	DBCoolListTextsRM* rm = new DBCoolListTextsRM(dbm_, DBRecordManager::CoolListTexts, decorated_->createCoolListTexts());

	Debug::trace("DBFactory.create", "Created DBCoolListTextsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBCoolPreTextsRM* DBFactory::createCoolPreTexts()
{
	DBCoolPreTextsRM* rm = new DBCoolPreTextsRM(dbm_, DBRecordManager::CoolPreTexts, decorated_->createCoolPreTexts());

	Debug::trace("DBFactory.create", "Created DBCoolPreTextsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBCoolProductionStepsRM* DBFactory::createCoolProductionSteps()
{
	DBCoolProductionStepsRM* rm = new DBCoolProductionStepsRM(dbm_, DBRecordManager::CoolProductionSteps, decorated_->createCoolProductionSteps());

	Debug::trace("DBFactory.create", "Created DBCoolProductionStepsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBCoolTextsRM* DBFactory::createCoolTexts()
{
	DBCoolTextsRM* rm = new DBCoolTextsRM(dbm_, DBRecordManager::CoolTexts, decorated_->createCoolTexts());

	Debug::trace("DBFactory.create", "Created DBCoolTextsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBCuttingTestsRM* DBFactory::createCuttingTests()
{
	DBCuttingTestsRM* rm = new DBCuttingTestsRM(dbm_, DBRecordManager::CuttingTests, decorated_->createCuttingTests());

	Debug::trace("DBFactory.create", "Created DBCuttingTestsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBDeptsRM* DBFactory::createDepts()
{
	DBDeptsRM* rm = new DBDeptsRM(dbm_, DBRecordManager::Depts, decorated_->createDepts());

	Debug::trace("DBFactory.create", "Created DBDeptsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBDevicesRM* DBFactory::createDevices()
{
	DBDevicesRM* rm = new DBDevicesRM(dbm_, DBRecordManager::Devices, decorated_->createDevices());

	Debug::trace("DBFactory.create", "Created DBDevicesRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBEmailAlertsRM* DBFactory::createEmailAlerts()
{
	DBEmailAlertsRM* rm = new DBEmailAlertsRM(dbm_, DBRecordManager::EmailAlerts, decorated_->createEmailAlerts());

	Debug::trace("DBFactory.create", "Created DBEmailAlertsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBExpirationLabelsRM* DBFactory::createExpirationLabels()
{
	DBExpirationLabelsRM* rm = new DBExpirationLabelsRM(dbm_, DBRecordManager::ExpirationLabels, decorated_->createExpirationLabels());

	Debug::trace("DBFactory.create", "Created DBExpirationLabelsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBFlashFilesRM* DBFactory::createFlashFiles()
{
	DBFlashFilesRM* rm = new DBFlashFilesRM(dbm_, DBRecordManager::FlashFiles, decorated_->createFlashFiles());

	Debug::trace("DBFactory.create", "Created DBFlashFilesRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBKnifeTimeHistoryRM* DBFactory::createWrapperKnife()
{
	DBKnifeTimeHistoryRM* rm = new DBKnifeTimeHistoryRM(dbm_, DBRecordManager::WrapperKnife, decorated_->createWrapperKnife());

	Debug::trace("DBFactory.create", "Created DBKnifeTimeHistoryRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBLabelsRM* DBFactory::createLabels()
{
	DBLabelsRM* rm = new DBLabelsRM(dbm_, DBRecordManager::Labels, decorated_->createLabels());

	Debug::trace("DBFactory.create", "Created DBLabelsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBLabelTextsRM* DBFactory::createLabelTexts()
{
	DBLabelTextsRM* rm = new DBLabelTextsRM(dbm_, DBRecordManager::LabelTexts, decorated_->createLabelTexts());

	Debug::trace("DBFactory.create", "Created DBLabelTextsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBNetworkErrorsRM* DBFactory::createNetworkErrors()
{
	DBNetworkErrorsRM* rm = new DBNetworkErrorsRM(dbm_, DBRecordManager::NetworkErrors, decorated_->createNetworkErrors());

	Debug::trace("DBFactory.create", "Created DBNetworkErrorsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBNutritionsRM* DBFactory::createNutritions()
{
	DBNutritionsRM* rm = new DBNutritionsRM(dbm_, DBRecordManager::Nutritions, decorated_->createNutritions());

	Debug::trace("DBFactory.create", "Created DBNutritionsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBOperatorsRM* DBFactory::createOperators()
{
	DBOperatorsRM* rm = new DBOperatorsRM(dbm_, DBRecordManager::Operators, decorated_->createOperators());

	Debug::trace("DBFactory.create", "Created DBOperatorsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBOperatorNotesRM* DBFactory::createOperatorNotes()
{
	DBOperatorNotesRM* rm = new DBOperatorNotesRM(dbm_, DBRecordManager::OperatorNotes, decorated_->createOperatorNotes());

	Debug::trace("DBFactory.create", "Created DBOperatorNotesRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBPresentationMethodsRM* DBFactory::createPresentationMethods()
{
	DBPresentationMethodsRM* rm = new DBPresentationMethodsRM(dbm_, DBRecordManager::PresentationMethods, decorated_->createPresentationMethods());

	Debug::trace("DBFactory.create", "Created DBPresentationMethodsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBPresentationMethodDetailedStatisticsRM* DBFactory::createPresentationMethodDetailedStatistics()
{
	DBPresentationMethodDetailedStatisticsRM* rm = new DBPresentationMethodDetailedStatisticsRM(dbm_, DBRecordManager::PresentationMethodDetailedStatistics, decorated_->createPresentationMethodDetailedStatistics());

	Debug::trace("DBFactory.create", "Created DBPresentationMethodDetailedStatisticsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBPresentationMethodStatisticsRM* DBFactory::createPresentationMethodStatistics()
{
	DBPresentationMethodStatisticsRM* rm = new DBPresentationMethodStatisticsRM(dbm_, DBRecordManager::PresentationMethodStatistics, decorated_->createPresentationMethodStatistics());

	Debug::trace("DBFactory.create", "Created DBPresentationMethodStatisticsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBProductsRM* DBFactory::createProducts()
{
	DBProductsRM* rm = new DBProductsRM(dbm_, DBRecordManager::Products, decorated_->createProducts());

	Debug::trace("DBFactory.create", "Created DBProductsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBProductDefaultsRM* DBFactory::createProductDefaults()
{
	DBProductDefaultsRM* rm = new DBProductDefaultsRM(dbm_, DBRecordManager::ProductDefaults, decorated_->createProductDefaults());

	Debug::trace("DBFactory.create", "Created DBProductDefaultsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBProductDetailedStatisticsRM* DBFactory::createProductDetailedStatistics()
{
	DBProductDetailedStatisticsRM* rm = new DBProductDetailedStatisticsRM(dbm_, DBRecordManager::ProductDetailedStatistics, decorated_->createProductDetailedStatistics());

	Debug::trace("DBFactory.create", "Created DBProductDetailedStatisticsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBProductStatisticsRM* DBFactory::createProductStatistics()
{
	DBProductStatisticsRM* rm = new DBProductStatisticsRM(dbm_, DBRecordManager::ProductStatistics, decorated_->createProductStatistics());

	Debug::trace("DBFactory.create", "Created DBProductStatisticsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBProductTextsRM* DBFactory::createProductTexts()
{
	DBProductTextsRM* rm = new DBProductTextsRM(dbm_, DBRecordManager::ProductTexts, decorated_->createProductTexts());

	Debug::trace("DBFactory.create", "Created DBProductTextsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBProductTextCfgsRM* DBFactory::createProductTextCfgs()
{
	DBProductTextCfgsRM* rm = new DBProductTextCfgsRM(dbm_, DBRecordManager::ProductTextCfgs, decorated_->createProductTextCfgs());

	Debug::trace("DBFactory.create", "Created DBProductTextCfgsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBRecallsRM* DBFactory::createRecalls()
{
	DBRecallsRM* rm = new DBRecallsRM(dbm_, DBRecordManager::Recalls, decorated_->createRecalls());

	Debug::trace("DBFactory.create", "Created DBRecallsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBROMLabelsRM* DBFactory::createROMLabels()
{
	DBROMLabelsRM* rm = new DBROMLabelsRM(dbm_, DBRecordManager::ROMLabels, decorated_->createROMLabels());

	Debug::trace("DBFactory.create", "Created DBROMLabelsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBScalesRM* DBFactory::createScales()
{
	DBScalesRM* rm = new DBScalesRM(dbm_, DBRecordManager::Scales, decorated_->createScales());

	Debug::trace("DBFactory.create", "Created DBScalesRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBSoftKeysRM* DBFactory::createSoftKeys()
{
	DBSoftKeysRM* rm = new DBSoftKeysRM(dbm_, DBRecordManager::SoftKeys, decorated_->createSoftKeys());

	Debug::trace("DBFactory.create", "Created DBSoftKeysRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBSoftKeyFeaturesRM* DBFactory::createSoftKeyFeatures()
{
	DBSoftKeyFeaturesRM* rm = new DBSoftKeyFeaturesRM(dbm_, DBRecordManager::SoftKeyFeatures, decorated_->createSoftKeyFeatures());

	Debug::trace("DBFactory.create", "Created DBSoftKeyFeaturesRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBSoftKeyGroupsRM* DBFactory::createSoftKeyGroups()
{
	DBSoftKeyGroupsRM* rm = new DBSoftKeyGroupsRM(dbm_, DBRecordManager::SoftKeyGroups, decorated_->createSoftKeyGroups());

	Debug::trace("DBFactory.create", "Created DBSoftKeyGroupsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBSoftKeyStylesRM* DBFactory::createSoftKeyStyles()
{
	DBSoftKeyStylesRM* rm = new DBSoftKeyStylesRM(dbm_, DBRecordManager::SoftKeyStyles, decorated_->createSoftKeyStyles());

	Debug::trace("DBFactory.create", "Created DBSoftKeyStylesRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBSpecialProgramsRM* DBFactory::createSpecialPrograms()
{
	DBSpecialProgramsRM* rm = new DBSpecialProgramsRM(dbm_, DBRecordManager::SpecialPrograms, decorated_->createSpecialPrograms());

	Debug::trace("DBFactory.create", "Created DBSpecialProgramsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBStatusRM* DBFactory::createStatus()
{
	DBStatusRM* rm = new DBStatusRM(dbm_, DBRecordManager::Status);

	Debug::trace("DBFactory.create", "Created DBStatusRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBStreamingGraphicsRM* DBFactory::createStreamingGraphics()
{
	DBStreamingGraphicsRM* rm = new DBStreamingGraphicsRM(dbm_, DBRecordManager::StreamingGraphics, decorated_->createStreamingGraphics());

	Debug::trace("DBFactory.create", "Created DBStreamingGraphicsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBSynchronizationChangesRM* DBFactory::createSynchronizationChanges()
{
	DBSynchronizationChangesRM* rm = new DBSynchronizationChangesRM(dbm_, DBRecordManager::SynchronizationChanges, decorated_->createSynchronizationChanges());

	Debug::trace("DBFactory.create", "Created DBSynchronizationChangesRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBTransactionsRM* DBFactory::createTransactions()
{
	DBTransactionsRM* rm = new DBTransactionsRM(dbm_, DBRecordManager::Transactions, decorated_->createTransactions());

	Debug::trace("DBFactory.create", "Created DBTransactionsRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBTransactionTypesRM* DBFactory::createTransactionTypes()
{
	DBTransactionTypesRM* rm = new DBTransactionTypesRM(dbm_, DBRecordManager::TransactionTypes, decorated_->createTransactionTypes());

	Debug::trace("DBFactory.create", "Created DBTransactionTypesRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


DBWrapperErrorHistoryRM* DBFactory::createWrapperErrorHistory()
{
	DBWrapperErrorHistoryRM* rm = new DBWrapperErrorHistoryRM(dbm_, DBRecordManager::WrapperErrorHistory, decorated_->createWrapperErrorHistory());

	Debug::trace("DBFactory.create", "Created DBWrapperErrorHistoryRM instance 0x" + util_string::ptrtostring(rm));

	return rm;
}


void DBFactory::disableDBAccess()
{
	DBBatchesRM::disableAllRMs();
	DBCategoriesRM::disableAllRMs();
	DBCategoryDetailedStatisticsRM::disableAllRMs();
	DBCategoryStatisticsRM::disableAllRMs();
	DBClassesRM::disableAllRMs();
	DBConfigurationsRM::disableAllRMs();
	DBCoolCategoriesRM::disableAllRMs();
	DBCoolCategoryCountriesRM::disableAllRMs();
	DBCoolCategoryPreTextsRM::disableAllRMs();
	DBCoolCategoryProductionStepsRM::disableAllRMs();
	DBCoolCountriesRM::disableAllRMs();
	DBCoolListsRM::disableAllRMs();
	DBCoolListTextsRM::disableAllRMs();
	DBCoolPreTextsRM::disableAllRMs();
	DBCoolProductionStepsRM::disableAllRMs();
	DBCoolTextsRM::disableAllRMs();
	DBCuttingTestsRM::disableAllRMs();
	DBDeptsRM::disableAllRMs();
	DBDevicesRM::disableAllRMs();
	DBEmailAlertsRM::disableAllRMs();
	DBExpirationLabelsRM::disableAllRMs();
	DBFlashFilesRM::disableAllRMs();
	DBLabelsRM::disableAllRMs();
	DBLabelTextsRM::disableAllRMs();
	DBNetworkErrorsRM::disableAllRMs();
	DBNutritionsRM::disableAllRMs();
	DBOperatorsRM::disableAllRMs();
	DBOperatorNotesRM::disableAllRMs();
	DBPresentationMethodsRM::disableAllRMs();
	DBPresentationMethodDetailedStatisticsRM::disableAllRMs();
	DBPresentationMethodStatisticsRM::disableAllRMs();
	DBProductsRM::disableAllRMs();
	DBProductDefaultsRM::disableAllRMs();
	DBProductDetailedStatisticsRM::disableAllRMs();
	DBProductStatisticsRM::disableAllRMs();
	DBProductTextsRM::disableAllRMs();
	DBRecallsRM::disableAllRMs();
	DBROMLabelsRM::disableAllRMs();
	DBScalesRM::disableAllRMs();
	DBSoftKeysRM::disableAllRMs();
	DBSoftKeyFeaturesRM::disableAllRMs();
	DBSoftKeyGroupsRM::disableAllRMs();
	DBSoftKeyStylesRM::disableAllRMs();
	DBSpecialProgramsRM::disableAllRMs();
	DBStreamingGraphicsRM::disableAllRMs();
	DBSynchronizationChangesRM::disableAllRMs();
	DBTransactionsRM::disableAllRMs();
	DBTransactionTypesRM::disableAllRMs();
}


void DBFactory::showDBRecordManagers(const std::string& dc)
{
	std::string debuggingCategory(dc);

	if ( debuggingCategory.empty() )
		debuggingCategory.assign("database.show");

	DBBatchesRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBCategoriesRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBCategoryDetailedStatisticsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBCategoryStatisticsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBClassesRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBConfigurationsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBCoolCategoriesRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBCoolCategoryCountriesRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBCoolCategoryPreTextsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBCoolCategoryProductionStepsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBCoolCountriesRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBCoolListsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBCoolListTextsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBCoolPreTextsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBCoolProductionStepsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBCoolTextsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBCuttingTestsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBDeptsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBDevicesRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBEmailAlertsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBExpirationLabelsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBFlashFilesRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBLabelsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBLabelTextsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBNetworkErrorsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBNutritionsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBOperatorsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBOperatorNotesRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBPresentationMethodsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBPresentationMethodDetailedStatisticsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBPresentationMethodStatisticsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBProductsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBProductDefaultsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBProductDetailedStatisticsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBProductStatisticsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBProductTextsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBRecallsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBROMLabelsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBScalesRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBSoftKeysRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBSoftKeyFeaturesRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBSoftKeyGroupsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBSoftKeyStylesRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBSpecialProgramsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBStreamingGraphicsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBSynchronizationChangesRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBTransactionsRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
	DBTransactionTypesRM::showAll(debuggingCategory);
	Debug::trace(debuggingCategory, 20, "");
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
