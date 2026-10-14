#ifndef DBFACTORY_H
#define DBFACTORY_H



#include <string>


#include "edb.h"

#include DBFACTORYI_INC



#ifndef NO_NAMESPACES
namespace edb
{
#endif


	class DBManager;
	class DBBatchesRM;
	class DBBarcodeScannersRM;
	class DBCategoriesRM;
	class DBCategoryDetailedStatisticsRM;
	class DBCategoryStatisticsRM;
	class DBClassesRM;
	class DBConfigurationsRM;
	class DBCoolCategoriesRM;
	class DBCoolCategoryCountriesRM;
	class DBCoolCategoryPreTextsRM;
	class DBCoolCategoryProductionStepsRM;
	class DBCoolCountriesRM;
	class DBCoolListsRM;
	class DBCoolListTextsRM;
	class DBCoolPreTextsRM;
	class DBCoolProductionStepsRM;
	class DBCoolTextsRM;
	class DBCuttingTestsRM;
	class DBDeptsRM;
	class DBDevicesRM;
	class DBEmailAlertsRM;
	class DBExpirationLabelsRM;
	class DBFlashFilesRM;
	class DBKnifeTimeHistoryRM;
	class DBLabelsRM;
	class DBLabelTextsRM;
	class DBNetworkErrorsRM;
	class DBNutritionsRM;
	class DBOperatorsRM;
	class DBOperatorNotesRM;
	class DBPresentationMethodsRM;
	class DBPresentationMethodDetailedStatisticsRM;
	class DBPresentationMethodStatisticsRM;
	class DBProductsRM;
	class DBProductDefaultsRM;
	class DBProductDetailedStatisticsRM;
	class DBProductStatisticsRM;
	class DBProductTextsRM;
	class DBProductTextCfgsRM;
	class DBRecallsRM;
	class DBROMLabelsRM;
	class DBScalesRM;
	class DBSoftKeysRM;
	class DBSoftKeyFeaturesRM;
	class DBSoftKeyGroupsRM;
	class DBSoftKeyStylesRM;
	class DBSpecialProgramsRM;
	class DBStatusRM;
	class DBStreamingGraphicsRM;
	class DBSynchronizationChangesRM;
	class DBTransactionsRM;
	class DBTransactionTypesRM;
	class DBWrapperErrorHistoryRM;


	// DBFactory is the class that implements the factory methods in terms of
	// lower database layers' functionality (each database layer must supply
	// its own Factory class upon which this one relies).
	class DBFactory
	{
		public:
			// --- Constructors and destructor ---
			DBFactory(DBManager* dbm, edb_adapter::DBFactoryI* f);
			virtual ~DBFactory();


			// --- Factory methods ---
			DBBatchesRM*                               createBatches();
            DBBarcodeScannersRM*                       createBarcodeScanners();
			DBCategoriesRM*                            createCategories();
			DBCategoryDetailedStatisticsRM*            createCategoryDetailedStatistics();
			DBCategoryStatisticsRM*                    createCategoryStatistics();
			DBClassesRM*                               createClasses();
			DBConfigurationsRM*                        createConfigurations();
			DBCoolCategoriesRM*                        createCoolCategories();
			DBCoolCategoryCountriesRM*                 createCoolCategoryCountries();
			DBCoolCategoryPreTextsRM*                  createCoolCategoryPreTexts();
			DBCoolCategoryProductionStepsRM*           createCoolCategoryProductionSteps();
			DBCoolCountriesRM*                         createCoolCountries();
			DBCoolListsRM*                             createCoolLists();
			DBCoolListTextsRM*                         createCoolListTexts();
			DBCoolPreTextsRM*                          createCoolPreTexts();
			DBCoolProductionStepsRM*                   createCoolProductionSteps();
			DBCoolTextsRM*                             createCoolTexts();
			DBCuttingTestsRM*                          createCuttingTests();
			DBDeptsRM*                                 createDepts();
			DBDevicesRM*                               createDevices();
			DBEmailAlertsRM*                           createEmailAlerts();
			DBExpirationLabelsRM*                      createExpirationLabels();
			DBFlashFilesRM*                            createFlashFiles();
			DBKnifeTimeHistoryRM*                      createWrapperKnife();	
			DBLabelsRM*                                createLabels();
			DBLabelTextsRM*                            createLabelTexts();
			DBNetworkErrorsRM*                         createNetworkErrors();
			DBNutritionsRM*                            createNutritions();
			DBOperatorsRM*                             createOperators();
			DBOperatorNotesRM*                         createOperatorNotes();
			DBPresentationMethodsRM*                   createPresentationMethods();
			DBPresentationMethodDetailedStatisticsRM*  createPresentationMethodDetailedStatistics();
			DBPresentationMethodStatisticsRM*          createPresentationMethodStatistics();
			DBProductsRM*                              createProducts();
			DBProductDefaultsRM*                       createProductDefaults();
			DBProductDetailedStatisticsRM*             createProductDetailedStatistics();
			DBProductStatisticsRM*                     createProductStatistics();
			DBProductTextsRM*                          createProductTexts();
			DBProductTextCfgsRM*                       createProductTextCfgs();
			DBRecallsRM*                               createRecalls();
			DBROMLabelsRM*                             createROMLabels();
			DBScalesRM*                                createScales();
			DBSoftKeysRM*                              createSoftKeys();
			DBSoftKeyFeaturesRM*                       createSoftKeyFeatures();
			DBSoftKeyGroupsRM*                         createSoftKeyGroups();
			DBSoftKeyStylesRM*                         createSoftKeyStyles();
			DBSpecialProgramsRM*                       createSpecialPrograms();
			DBStatusRM*                                createStatus();
			DBStreamingGraphicsRM*                     createStreamingGraphics();
			DBSynchronizationChangesRM*                createSynchronizationChanges();
			DBTransactionsRM*                          createTransactions();
			DBTransactionTypesRM*                      createTransactionTypes();
			DBWrapperErrorHistoryRM*                   createWrapperErrorHistory();

			// Template method used for testing purposes
			template <class DBEntityRM> DBEntityRM* create();


			// --- Processing methods ---
			// Disables all record manager instances associated with all
			// Record Manager classes currently active
			void disableDBAccess();

			// Prints information for all record manager instances associated
			// with all Record Manager classes currently active
			static void showDBRecordManagers(const std::string& debuggingCategory);


		private:
			DBManager* dbm_;
			edb_adapter::DBFactoryI* decorated_;

			// Do not allow the default copy constructor.
			DBFactory(const DBFactory& f);
			// Do not allow the default assignment operator.
			DBFactory operator=(const DBFactory& rhs);
	};



#ifndef NO_NAMESPACES
} // namespace edb
#endif



#endif // DBFACTORY_H

