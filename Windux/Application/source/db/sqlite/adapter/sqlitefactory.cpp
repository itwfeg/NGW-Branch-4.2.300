#include "sqlitefactory.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif


#ifdef USE_ACCOUNTANT
int SqliteFactory::_dataType_ = 0;
std::string SqliteFactory::_dataTypeName_("SqliteFactory");
#endif



SqliteFactory::SqliteFactory(const SqliteDatabase& sdb, const util_env::Properties& props, DBFactoryI* f)
			: sdb_(sdb), config_(props), factoryMethods_(), decorated_(f)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	createFactoryMethods(); 
}


SqliteFactory::~SqliteFactory() 
{ 
	if ( decorated_ )
		delete decorated_; 
		
	deleteFactoryMethods(); 


#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


SqliteBatchesRM* SqliteFactory::createBatches()
{
	if ( decorated_ )
		return new SqliteBatchesRM(sdb_, config_, decorated_->createBatches());
	else
		return new SqliteBatchesRM(sdb_, config_);
}


SqliteCategoriesRM* SqliteFactory::createCategories()
{
	if ( decorated_ )
		return new SqliteCategoriesRM(sdb_, config_, decorated_->createCategories());
	else
		return new SqliteCategoriesRM(sdb_, config_);
}


SqliteCategoryStatisticsRM* SqliteFactory::createCategoryStatistics()
{
	if ( decorated_ )
		return new SqliteCategoryStatisticsRM(sdb_, config_, decorated_->createCategoryStatistics());
	else
		return new SqliteCategoryStatisticsRM(sdb_, config_);
}


SqliteCategoryDetailedStatisticsRM* SqliteFactory::createCategoryDetailedStatistics()
{
	if ( decorated_ )
		return new SqliteCategoryDetailedStatisticsRM(sdb_, config_, decorated_->createCategoryDetailedStatistics());
	else
		return new SqliteCategoryDetailedStatisticsRM(sdb_, config_);
}


SqliteClassesRM* SqliteFactory::createClasses()
{
	if ( decorated_ )
		return new SqliteClassesRM(sdb_, config_, decorated_->createClasses());
	else
		return new SqliteClassesRM(sdb_, config_);
}

SqliteCoolCountriesRM* SqliteFactory::createCoolCountries()
{
	if (decorated_)
		return new SqliteCoolCountriesRM(sdb_, config_, decorated_->createCoolCountries());
	else
		return new SqliteCoolCountriesRM(sdb_, config_);
}

SqliteConfigurationsRM* SqliteFactory::createConfigurations()
{
	if ( decorated_ )
		return new SqliteConfigurationsRM(sdb_, config_, decorated_->createConfigurations());
	else
		return new SqliteConfigurationsRM(sdb_, config_);
}


SqliteCoolCategoriesRM* SqliteFactory::createCoolCategories()
{
	if ( decorated_ )
		return new SqliteCoolCategoriesRM(sdb_, config_, decorated_->createCoolCategories());
	else
		return new SqliteCoolCategoriesRM(sdb_, config_);
}


SqliteCoolCategoryCountriesRM* SqliteFactory::createCoolCategoryCountries()
{
	if ( decorated_ )
		return new SqliteCoolCategoryCountriesRM(sdb_, config_, decorated_->createCoolCategoryCountries());
	else
		return new SqliteCoolCategoryCountriesRM(sdb_, config_);
}


SqliteCoolCategoryPreTextsRM* SqliteFactory::createCoolCategoryPreTexts()
{
	if ( decorated_ )
		return new SqliteCoolCategoryPreTextsRM(sdb_, config_, decorated_->createCoolCategoryPreTexts());
	else
		return new SqliteCoolCategoryPreTextsRM(sdb_, config_);
}


SqliteCoolCategoryProductionStepsRM* SqliteFactory::createCoolCategoryProductionSteps()
{
	if ( decorated_ )
		return new SqliteCoolCategoryProductionStepsRM(sdb_, config_, decorated_->createCoolCategoryProductionSteps());
	else
		return new SqliteCoolCategoryProductionStepsRM(sdb_, config_);
}


SqliteCoolListsRM* SqliteFactory::createCoolLists()
{
	if ( decorated_ )
		return new SqliteCoolListsRM(sdb_, config_, decorated_->createCoolLists());
	else
		return new SqliteCoolListsRM(sdb_, config_);
}


SqliteCoolListTextsRM* SqliteFactory::createCoolListTexts()
{
	if ( decorated_ )
		return new SqliteCoolListTextsRM(sdb_, config_, decorated_->createCoolListTexts());
	else
		return new SqliteCoolListTextsRM(sdb_, config_);
}


SqliteCoolPreTextsRM* SqliteFactory::createCoolPreTexts()
{
	if ( decorated_ )
		return new SqliteCoolPreTextsRM(sdb_, config_, decorated_->createCoolPreTexts());
	else
		return new SqliteCoolPreTextsRM(sdb_, config_);
}


SqliteCoolProductionStepsRM* SqliteFactory::createCoolProductionSteps()
{
	if ( decorated_ )
		return new SqliteCoolProductionStepsRM(sdb_, config_, decorated_->createCoolProductionSteps());
	else
		return new SqliteCoolProductionStepsRM(sdb_, config_);
}


SqliteCoolTextsRM* SqliteFactory::createCoolTexts()
{
	if ( decorated_ )
		return new SqliteCoolTextsRM(sdb_, config_, decorated_->createCoolTexts());
	else
		return new SqliteCoolTextsRM(sdb_, config_);
}


SqliteCuttingTestsRM* SqliteFactory::createCuttingTests()
{
	if ( decorated_ )
		return new SqliteCuttingTestsRM(sdb_, config_, decorated_->createCuttingTests());
	else
		return new SqliteCuttingTestsRM(sdb_, config_);
}


SqliteDeptsRM* SqliteFactory::createDepts()
{
	if (decorated_)
		return new SqliteDeptsRM(sdb_, config_, decorated_->createDepts());
	else
		return new SqliteDeptsRM(sdb_, config_);
}


SqliteDevicesRM* SqliteFactory::createDevices()
{
	if ( decorated_ )
		return new SqliteDevicesRM(sdb_, config_, decorated_->createDevices());
	else
		return new SqliteDevicesRM(sdb_, config_);
}


SqliteEmailAlertsRM* SqliteFactory::createEmailAlerts()
{
	if ( decorated_ )
		return new SqliteEmailAlertsRM(sdb_, config_, decorated_->createEmailAlerts());
	else
		return new SqliteEmailAlertsRM(sdb_, config_);
}


SqliteExpirationLabelsRM* SqliteFactory::createExpirationLabels()
{
	if ( decorated_ )
		return new SqliteExpirationLabelsRM(sdb_, config_, decorated_->createExpirationLabels());
	else
		return new SqliteExpirationLabelsRM(sdb_, config_);
}


SqliteFlashFilesRM* SqliteFactory::createFlashFiles()
{
	if ( decorated_ )
		return new SqliteFlashFilesRM(sdb_, config_, decorated_->createFlashFiles());
	else
		return new SqliteFlashFilesRM(sdb_, config_);
}


SqliteLabelsRM* SqliteFactory::createLabels()
{
	if ( decorated_ )
		return new SqliteLabelsRM(sdb_, config_, decorated_->createLabels());
	else
		return new SqliteLabelsRM(sdb_, config_);
}


SqliteLabelTextsRM* SqliteFactory::createLabelTexts()
{
	if ( decorated_ )
		return new SqliteLabelTextsRM(sdb_, config_, decorated_->createLabelTexts());
	else
		return new SqliteLabelTextsRM(sdb_, config_);
}


SqliteNetworkErrorsRM* SqliteFactory::createNetworkErrors()
{
	if ( decorated_ )
		return new SqliteNetworkErrorsRM(sdb_, config_, decorated_->createNetworkErrors());
	else
		return new SqliteNetworkErrorsRM(sdb_, config_);
}


SqliteNutritionsRM* SqliteFactory::createNutritions()
{
	if ( decorated_ )
		return new SqliteNutritionsRM(sdb_, config_, decorated_->createNutritions());
	else
		return new SqliteNutritionsRM(sdb_, config_);
}


SqliteOperatorsRM* SqliteFactory::createOperators()
{
	if ( decorated_ )
		return new SqliteOperatorsRM(sdb_, config_, decorated_->createOperators());
	else
		return new SqliteOperatorsRM(sdb_, config_);
}


SqliteOperatorNotesRM* SqliteFactory::createOperatorNotes()
{
	if ( decorated_ )
		return new SqliteOperatorNotesRM(sdb_, config_, decorated_->createOperatorNotes());
	else
		return new SqliteOperatorNotesRM(sdb_, config_);
}


SqlitePresentationMethodsRM* SqliteFactory::createPresentationMethods()
{
	if ( decorated_ )
		return new SqlitePresentationMethodsRM(sdb_, config_, decorated_->createPresentationMethods());
	else
		return new SqlitePresentationMethodsRM(sdb_, config_);
}


SqlitePresentationMethodStatisticsRM* SqliteFactory::createPresentationMethodStatistics()
{
	if ( decorated_ )
		return new SqlitePresentationMethodStatisticsRM(sdb_, config_, decorated_->createPresentationMethodStatistics());
	else
		return new SqlitePresentationMethodStatisticsRM(sdb_, config_);
}


SqlitePresentationMethodDetailedStatisticsRM* SqliteFactory::createPresentationMethodDetailedStatistics()
{
	if ( decorated_ )
		return new SqlitePresentationMethodDetailedStatisticsRM(sdb_, config_, decorated_->createPresentationMethodDetailedStatistics());
	else
		return new SqlitePresentationMethodDetailedStatisticsRM(sdb_, config_);
}


SqliteProductsRM* SqliteFactory::createProducts()
{
	if ( decorated_ )
		return new SqliteProductsRM(sdb_, config_, decorated_->createProducts());
	else
		return new SqliteProductsRM(sdb_, config_);
}


SqliteProductDefaultsRM* SqliteFactory::createProductDefaults()
{
	if ( decorated_ )
		return new SqliteProductDefaultsRM(sdb_, config_, decorated_->createProductDefaults());
	else
		return new SqliteProductDefaultsRM(sdb_, config_);
}


SqliteProductStatisticsRM* SqliteFactory::createProductStatistics()
{
	if ( decorated_ )
		return new SqliteProductStatisticsRM(sdb_, config_, decorated_->createProductStatistics());
	else
		return new SqliteProductStatisticsRM(sdb_, config_);
}


SqliteProductDetailedStatisticsRM* SqliteFactory::createProductDetailedStatistics()
{
	if ( decorated_ )
		return new SqliteProductDetailedStatisticsRM(sdb_, config_, decorated_->createProductDetailedStatistics());
	else
		return new SqliteProductDetailedStatisticsRM(sdb_, config_);
}


SqliteProductTextsRM* SqliteFactory::createProductTexts()
{
	if ( decorated_ )
		return new SqliteProductTextsRM(sdb_, config_, decorated_->createProductTexts());
	else
		return new SqliteProductTextsRM(sdb_, config_);
}


SqliteProductTextCfgsRM* SqliteFactory::createProductTextCfgs()
{
	if ( decorated_ )
		return new SqliteProductTextCfgsRM(sdb_, config_, decorated_->createProductTextCfgs());
	else
		return new SqliteProductTextCfgsRM(sdb_, config_);
}

SqliteBarcodeScannersRM* SqliteFactory::createBarcodeScanners()
{
	if ( decorated_ )
		return new SqliteBarcodeScannersRM(sdb_, config_, decorated_->createBarcodeScanners());
	else
		return new SqliteBarcodeScannersRM(sdb_, config_);
}

SqliteRecallsRM* SqliteFactory::createRecalls()
{
	if ( decorated_ )
		return new SqliteRecallsRM(sdb_, config_, decorated_->createRecalls());
	else
		return new SqliteRecallsRM(sdb_, config_);
}


SqliteROMLabelsRM* SqliteFactory::createROMLabels()
{
	if ( decorated_ )
		return new SqliteROMLabelsRM(sdb_, config_, decorated_->createROMLabels());
	else
		return new SqliteROMLabelsRM(sdb_, config_);
}


SqliteScalesRM* SqliteFactory::createScales()
{
	if ( decorated_ )
		return new SqliteScalesRM(sdb_, config_, decorated_->createScales());
	else
		return new SqliteScalesRM(sdb_, config_);
}


SqliteSoftKeysRM* SqliteFactory::createSoftKeys()
{
	if ( decorated_ )
		return new SqliteSoftKeysRM(sdb_, config_, decorated_->createSoftKeys());
	else
		return new SqliteSoftKeysRM(sdb_, config_);
}


SqliteSoftKeyFeaturesRM* SqliteFactory::createSoftKeyFeatures()
{
	if ( decorated_ )
		return new SqliteSoftKeyFeaturesRM(sdb_, config_, decorated_->createSoftKeyFeatures());
	else
		return new SqliteSoftKeyFeaturesRM(sdb_, config_);
}


SqliteSoftKeyGroupsRM* SqliteFactory::createSoftKeyGroups()
{
	if ( decorated_ )
		return new SqliteSoftKeyGroupsRM(sdb_, config_, decorated_->createSoftKeyGroups());
	else
		return new SqliteSoftKeyGroupsRM(sdb_, config_);
}


SqliteSoftKeyStylesRM* SqliteFactory::createSoftKeyStyles()
{
	if ( decorated_ )
		return new SqliteSoftKeyStylesRM(sdb_, config_, decorated_->createSoftKeyStyles());
	else
		return new SqliteSoftKeyStylesRM(sdb_, config_);
}


SqliteSpecialProgramsRM* SqliteFactory::createSpecialPrograms()
{
	if ( decorated_ )
		return new SqliteSpecialProgramsRM(sdb_, config_, decorated_->createSpecialPrograms());
	else
		return new SqliteSpecialProgramsRM(sdb_, config_);
}


SqliteStreamingGraphicsRM* SqliteFactory::createStreamingGraphics()
{
	if ( decorated_ )
		return new SqliteStreamingGraphicsRM(sdb_, config_, decorated_->createStreamingGraphics());
	else
		return new SqliteStreamingGraphicsRM(sdb_, config_);
}


SqliteSynchronizationChangesRM* SqliteFactory::createSynchronizationChanges()
{
	if ( decorated_ )
		return new SqliteSynchronizationChangesRM(sdb_, config_, decorated_->createSynchronizationChanges());
	else
		return new SqliteSynchronizationChangesRM(sdb_, config_);
}


SqliteTransactionsRM* SqliteFactory::createTransactions()
{
	if ( decorated_ )
		return new SqliteTransactionsRM(sdb_, config_, decorated_->createTransactions());
	else
		return new SqliteTransactionsRM(sdb_, config_);
}


SqliteTransactionTypesRM* SqliteFactory::createTransactionTypes()
{
	if ( decorated_ )
		return new SqliteTransactionTypesRM(sdb_, config_, decorated_->createTransactionTypes());
	else
		return new SqliteTransactionTypesRM(sdb_, config_);
}


SqliteWrapperErrorHistoryRM* SqliteFactory::createWrapperErrorHistory()
{
	if ( decorated_ )
		return new SqliteWrapperErrorHistoryRM(sdb_, config_, decorated_->createWrapperErrorHistory());
	else
		return new SqliteWrapperErrorHistoryRM(sdb_, config_);
}


SqliteKnifeTimeHistoryRM* SqliteFactory::createWrapperKnife()
{
	if ( decorated_ )
		return new SqliteKnifeTimeHistoryRM(sdb_, config_, decorated_->createWrapperKnife());
	else
		return new SqliteKnifeTimeHistoryRM(sdb_, config_);
}



void SqliteFactory::createFactoryMethods()
{
	if ( factoryMethods_.size() )
		factoryMethods_.clear();

	factoryMethods_.push_back(new FactoryFunctor<SqliteBatchesRM>                               (this, &SqliteFactory::createBatches));
	factoryMethods_.push_back(new FactoryFunctor<SqliteBarcodeScannersRM>                       (this, &SqliteFactory::createBarcodeScanners));
	factoryMethods_.push_back(new FactoryFunctor<SqliteCategoriesRM>                            (this, &SqliteFactory::createCategories));
	factoryMethods_.push_back(new FactoryFunctor<SqliteCategoryDetailedStatisticsRM>            (this, &SqliteFactory::createCategoryDetailedStatistics));
	factoryMethods_.push_back(new FactoryFunctor<SqliteCategoryStatisticsRM>                    (this, &SqliteFactory::createCategoryStatistics));
	factoryMethods_.push_back(new FactoryFunctor<SqliteClassesRM>                               (this, &SqliteFactory::createClasses));
	factoryMethods_.push_back(new FactoryFunctor<SqliteConfigurationsRM>                        (this, &SqliteFactory::createConfigurations));
	factoryMethods_.push_back(new FactoryFunctor<SqliteCoolCategoriesRM>                        (this, &SqliteFactory::createCoolCategories));
	factoryMethods_.push_back(new FactoryFunctor<SqliteCoolCategoryCountriesRM>                 (this, &SqliteFactory::createCoolCategoryCountries));
	factoryMethods_.push_back(new FactoryFunctor<SqliteCoolCategoryPreTextsRM>                  (this, &SqliteFactory::createCoolCategoryPreTexts));
	factoryMethods_.push_back(new FactoryFunctor<SqliteCoolCategoryProductionStepsRM>           (this, &SqliteFactory::createCoolCategoryProductionSteps));
	factoryMethods_.push_back(new FactoryFunctor<SqliteCoolCountriesRM>                         (this, &SqliteFactory::createCoolCountries));
	factoryMethods_.push_back(new FactoryFunctor<SqliteCoolListsRM>                             (this, &SqliteFactory::createCoolLists));
	factoryMethods_.push_back(new FactoryFunctor<SqliteCoolListTextsRM>                         (this, &SqliteFactory::createCoolListTexts));
	factoryMethods_.push_back(new FactoryFunctor<SqliteCoolPreTextsRM>                          (this, &SqliteFactory::createCoolPreTexts));
	factoryMethods_.push_back(new FactoryFunctor<SqliteCoolProductionStepsRM>                   (this, &SqliteFactory::createCoolProductionSteps));
	factoryMethods_.push_back(new FactoryFunctor<SqliteCoolTextsRM>                             (this, &SqliteFactory::createCoolTexts));
	factoryMethods_.push_back(new FactoryFunctor<SqliteCuttingTestsRM>                          (this, &SqliteFactory::createCuttingTests));
	factoryMethods_.push_back(new FactoryFunctor<SqliteDeptsRM>                                 (this, &SqliteFactory::createDepts));
	factoryMethods_.push_back(new FactoryFunctor<SqliteDevicesRM>                               (this, &SqliteFactory::createDevices));
	factoryMethods_.push_back(new FactoryFunctor<SqliteEmailAlertsRM>                           (this, &SqliteFactory::createEmailAlerts));
	factoryMethods_.push_back(new FactoryFunctor<SqliteExpirationLabelsRM>                      (this, &SqliteFactory::createExpirationLabels));
	factoryMethods_.push_back(new FactoryFunctor<SqliteFlashFilesRM>                            (this, &SqliteFactory::createFlashFiles));
	factoryMethods_.push_back(new FactoryFunctor<SqliteKnifeTimeHistoryRM>                      (this, &SqliteFactory::createWrapperKnife));
	factoryMethods_.push_back(new FactoryFunctor<SqliteLabelsRM>                                (this, &SqliteFactory::createLabels));
	factoryMethods_.push_back(new FactoryFunctor<SqliteLabelTextsRM>                            (this, &SqliteFactory::createLabelTexts));
	factoryMethods_.push_back(new FactoryFunctor<SqliteNetworkErrorsRM>                         (this, &SqliteFactory::createNetworkErrors));
	factoryMethods_.push_back(new FactoryFunctor<SqliteNutritionsRM>                            (this, &SqliteFactory::createNutritions));
	factoryMethods_.push_back(new FactoryFunctor<SqliteOperatorsRM>                             (this, &SqliteFactory::createOperators));
	factoryMethods_.push_back(new FactoryFunctor<SqliteOperatorNotesRM>                         (this, &SqliteFactory::createOperatorNotes));
	factoryMethods_.push_back(new FactoryFunctor<SqlitePresentationMethodsRM>                   (this, &SqliteFactory::createPresentationMethods));
	factoryMethods_.push_back(new FactoryFunctor<SqlitePresentationMethodDetailedStatisticsRM>  (this, &SqliteFactory::createPresentationMethodDetailedStatistics));
	factoryMethods_.push_back(new FactoryFunctor<SqlitePresentationMethodStatisticsRM>          (this, &SqliteFactory::createPresentationMethodStatistics));
	factoryMethods_.push_back(new FactoryFunctor<SqliteProductsRM>                              (this, &SqliteFactory::createProducts));
	factoryMethods_.push_back(new FactoryFunctor<SqliteProductDefaultsRM>                       (this, &SqliteFactory::createProductDefaults));
	factoryMethods_.push_back(new FactoryFunctor<SqliteProductDetailedStatisticsRM>             (this, &SqliteFactory::createProductDetailedStatistics));
	factoryMethods_.push_back(new FactoryFunctor<SqliteProductStatisticsRM>                     (this, &SqliteFactory::createProductStatistics));
	factoryMethods_.push_back(new FactoryFunctor<SqliteProductTextsRM>                          (this, &SqliteFactory::createProductTexts));
	factoryMethods_.push_back(new FactoryFunctor<SqliteProductTextCfgsRM>                       (this, &SqliteFactory::createProductTextCfgs));
	factoryMethods_.push_back(new FactoryFunctor<SqliteRecallsRM>                               (this, &SqliteFactory::createRecalls));
	factoryMethods_.push_back(new FactoryFunctor<SqliteROMLabelsRM>                             (this, &SqliteFactory::createROMLabels));
	factoryMethods_.push_back(new FactoryFunctor<SqliteScalesRM>                                (this, &SqliteFactory::createScales));
	factoryMethods_.push_back(new FactoryFunctor<SqliteSoftKeysRM>                              (this, &SqliteFactory::createSoftKeys));
	factoryMethods_.push_back(new FactoryFunctor<SqliteSoftKeyFeaturesRM>                       (this, &SqliteFactory::createSoftKeyFeatures));
	factoryMethods_.push_back(new FactoryFunctor<SqliteSoftKeyGroupsRM>                         (this, &SqliteFactory::createSoftKeyGroups));
	factoryMethods_.push_back(new FactoryFunctor<SqliteSoftKeyStylesRM>                         (this, &SqliteFactory::createSoftKeyStyles));
	factoryMethods_.push_back(new FactoryFunctor<SqliteSpecialProgramsRM>                       (this, &SqliteFactory::createSpecialPrograms));
	factoryMethods_.push_back(new FactoryFunctor<SqliteStreamingGraphicsRM>                     (this, &SqliteFactory::createStreamingGraphics));
	factoryMethods_.push_back(new FactoryFunctor<SqliteSynchronizationChangesRM>                (this, &SqliteFactory::createSynchronizationChanges));
	factoryMethods_.push_back(new FactoryFunctor<SqliteTransactionsRM>                          (this, &SqliteFactory::createTransactions));
	factoryMethods_.push_back(new FactoryFunctor<SqliteTransactionTypesRM>                      (this, &SqliteFactory::createTransactionTypes));
	factoryMethods_.push_back(new FactoryFunctor<SqliteWrapperErrorHistoryRM>                   (this, &SqliteFactory::createWrapperErrorHistory));
}


void SqliteFactory::deleteFactoryMethods()
{
	// Delete all the factory methods individually before deleting the vector.
	factoryMethodContainer_type::const_iterator iter = factoryMethods_.begin();
	while ( iter != factoryMethods_.end() )
		delete *iter++;
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
