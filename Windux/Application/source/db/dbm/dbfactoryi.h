#ifndef DBFACTORYI_H
#define DBFACTORYI_H


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBBatchesRMI;
	class DBBarcodeScannersRMI;
	class DBCategoriesRMI;
	class DBCategoryStatisticsRMI;
	class DBCategoryDetailedStatisticsRMI;
	class DBClassesRMI;
	class DBConfigurationsRMI;
	class DBCoolCategoriesRMI;
	class DBCoolCategoryCountriesRMI;
	class DBCoolCategoryPreTextsRMI;
	class DBCoolCategoryProductionStepsRMI;
	class DBCoolCountriesRMI;
	class DBCoolListsRMI;
	class DBCoolListTextsRMI;
	class DBCoolPreTextsRMI;
	class DBCoolProductionStepsRMI;
	class DBCoolTextsRMI;
	class DBCuttingTestsRMI;
	class DBDeptsRMI;
	class DBDevicesRMI;
	class DBEmailAlertsRMI;
	class DBExpirationLabelsRMI;
	class DBFlashFilesRMI;
	class DBKnifeTimeHistoryRMI;
	class DBLabelsRMI;
	class DBLabelTextsRMI;
	class DBNetworkErrorsRMI;
	class DBNutritionsRMI;
	class DBOperatorsRMI;
	class DBOperatorNotesRMI;
	class DBPresentationMethodsRMI;
	class DBPresentationMethodStatisticsRMI;
	class DBPresentationMethodDetailedStatisticsRMI;
	class DBProductsRMI;
	class DBProductDefaultsRMI;
	class DBProductStatisticsRMI;
	class DBProductDetailedStatisticsRMI;
	class DBProductTextsRMI;
	class DBProductTextCfgsRMI;
	class DBRecallsRMI;
	class DBROMLabelsRMI;
	class DBScalesRMI;
	class DBSoftKeysRMI;
	class DBSoftKeyFeaturesRMI;
	class DBSoftKeyGroupsRMI;
	class DBSoftKeyStylesRMI;
	class DBSpecialProgramsRMI;
	class DBStreamingGraphicsRMI;
	class DBSynchronizationChangesRMI;
	class DBTransactionsRMI;
	class DBTransactionTypesRMI;
	class DBWrapperErrorHistoryRMI;


	class DBFactoryI
	{
		public:

			virtual ~DBFactoryI() {}


			virtual DBBatchesRMI*                               createBatches() { return 0; }// = 0;
			virtual DBBarcodeScannersRMI*                       createBarcodeScanners() { return 0; }// = 0;
			virtual DBCategoriesRMI*                            createCategories() { return 0; }// = 0;
			virtual DBCategoryStatisticsRMI*                    createCategoryStatistics() { return 0; }// = 0;
			virtual DBCategoryDetailedStatisticsRMI*            createCategoryDetailedStatistics() { return 0; }// = 0;
			virtual DBClassesRMI*                               createClasses() { return 0; }// = 0;
			virtual DBConfigurationsRMI*                        createConfigurations() { return 0; }// = 0;
			virtual DBCoolCategoriesRMI*                        createCoolCategories() { return 0; }// = 0;
			virtual DBCoolCategoryCountriesRMI*                 createCoolCategoryCountries() { return 0; }// = 0;
			virtual DBCoolCategoryPreTextsRMI*                  createCoolCategoryPreTexts() { return 0; }// = 0;
			virtual DBCoolCategoryProductionStepsRMI*           createCoolCategoryProductionSteps() { return 0; }// = 0;
			virtual DBCoolCountriesRMI*                         createCoolCountries() { return 0; }// = 0;
			virtual DBCoolListsRMI*                             createCoolLists() { return 0; }// = 0;
			virtual DBCoolListTextsRMI*                         createCoolListTexts() { return 0; }// = 0;
			virtual DBCoolPreTextsRMI*                          createCoolPreTexts() { return 0; }// = 0;
			virtual DBCoolProductionStepsRMI*                   createCoolProductionSteps() { return 0; }// = 0;
			virtual DBCoolTextsRMI*                             createCoolTexts() { return 0; }// = 0;
			virtual DBCuttingTestsRMI*                          createCuttingTests() { return 0; }// = 0;
			virtual DBDeptsRMI*                                 createDepts() { return 0; }// = 0;
			virtual DBDevicesRMI*                               createDevices() { return 0; }// = 0;
			virtual DBEmailAlertsRMI*                           createEmailAlerts() { return 0; }// = 0;
			virtual DBExpirationLabelsRMI*                      createExpirationLabels() { return 0; }// = 0;
			virtual DBFlashFilesRMI*                            createFlashFiles() { return 0; }// = 0;
			virtual DBKnifeTimeHistoryRMI*                      createWrapperKnife() { return 0; }// = 0;
			virtual DBLabelsRMI*                                createLabels() { return 0; }// = 0;
			virtual DBLabelTextsRMI*                            createLabelTexts() { return 0; }// = 0;
			virtual DBNetworkErrorsRMI*                         createNetworkErrors() { return 0; }// = 0;
			virtual DBNutritionsRMI*                            createNutritions() { return 0; }// = 0;
			virtual DBOperatorsRMI*                             createOperators() { return 0; }// = 0;
			virtual DBOperatorNotesRMI*                         createOperatorNotes() { return 0; }// = 0;
			virtual DBPresentationMethodsRMI*                   createPresentationMethods() { return 0; }// = 0;
			virtual DBPresentationMethodStatisticsRMI*          createPresentationMethodStatistics() { return 0; }// = 0;
			virtual DBPresentationMethodDetailedStatisticsRMI*  createPresentationMethodDetailedStatistics() { return 0; }// = 0;
			virtual DBProductsRMI*                              createProducts() { return 0; }// = 0;
			virtual DBProductDefaultsRMI*                       createProductDefaults() { return 0; }// = 0;
			virtual DBProductStatisticsRMI*                     createProductStatistics() { return 0; }// = 0;
			virtual DBProductDetailedStatisticsRMI*             createProductDetailedStatistics()  { return 0; }// = 0;
			virtual DBProductTextsRMI*                          createProductTexts() { return 0; }// = 0;
			virtual DBProductTextCfgsRMI*                       createProductTextCfgs() { return 0; }// = 0;
			virtual DBRecallsRMI*                               createRecalls() { return 0; }// = 0;
			virtual DBROMLabelsRMI*                             createROMLabels() { return 0; }// = 0;
			virtual DBScalesRMI*                                createScales() { return 0; }// = 0;
			virtual DBSoftKeysRMI*                              createSoftKeys() { return 0; }// = 0;
			virtual DBSoftKeyFeaturesRMI*                       createSoftKeyFeatures() { return 0; }// = 0;
			virtual DBSoftKeyGroupsRMI*                         createSoftKeyGroups() { return 0; }// = 0;
			virtual DBSoftKeyStylesRMI*                         createSoftKeyStyles() { return 0; }// = 0;
			virtual DBSpecialProgramsRMI*                       createSpecialPrograms() { return 0; }// = 0;
			virtual DBStreamingGraphicsRMI*                     createStreamingGraphics() { return 0; }// = 0;
			virtual DBSynchronizationChangesRMI*                createSynchronizationChanges() { return 0; }// = 0;
			virtual DBTransactionsRMI*                          createTransactions() { return 0; }// = 0;
			virtual DBTransactionTypesRMI*                      createTransactionTypes() { return 0; }// = 0;
			virtual DBWrapperErrorHistoryRMI*                   createWrapperErrorHistory() { return 0; }// = 0;
	};


#ifndef NO_NAMESPACES
}  // namespace edb_adapter
#endif


#endif // DBFACTORYI_H
