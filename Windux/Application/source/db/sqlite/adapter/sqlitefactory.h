#ifndef SQLITEFACTORY_H
#define SQLITEFACTORY_H


#include "edb.h"

#include DBFACTORYI_INC
#include PROPERTIES_INC


// Need all the include files because SqliteFactory extends DBFactoryI and this one defines
// the factory methods as returning DBEntitiesRM* instead of SqliteEntitiesRM*. Without the
// include files the compiler does not know that the SqliteEntitiesRM classes extend the
// corresponding DBEntitiesRM's.


#include "sqlitebatchesrm.h"
#include "sqlitebarcodescannersrm.h"
#include "sqlitecategoriesrm.h"
#include "sqlitecategorystatisticsrm.h"
#include "sqlitecategorydetailedstatisticsrm.h"
#include "sqliteclassesrm.h"
#include "sqliteconfigurationsrm.h"
#include "sqlitecoolcategoriesrm.h"
#include "sqlitecoolcategorycountriesrm.h"
#include "sqlitecoolcategorypretextsrm.h"
#include "sqlitecoolcategoryproductionstepsrm.h"
#include "sqlitecoolcountriesrm.h"
#include "sqlitecoollistsrm.h"
#include "sqlitecoollisttextsrm.h"
#include "sqlitecoolpretextsrm.h"
#include "sqlitecoolproductionstepsrm.h"
#include "sqlitecooltextsrm.h"
#include "sqlitecuttingtestsrm.h"
#include "sqlitedeptsrm.h"
#include "sqlitedevicesrm.h"
#include "sqliteemailalertsrm.h"
#include "sqliteexpirationlabelsrm.h"
#include "sqliteflashfilesrm.h"
#include "sqliteknifetimehistoryrm.h"
#include "sqlitelabelsrm.h"
#include "sqlitelabeltextsrm.h"
#include "sqlitenetworkerrorsrm.h"
#include "sqlitenutritionsrm.h"
#include "sqliteoperatorsrm.h"
#include "sqliteoperatornotesrm.h"
#include "sqlitepresentationmethodsrm.h"
#include "sqlitepresentationmethodstatisticsrm.h"
#include "sqlitepresentationmethoddetailedstatisticsrm.h"
#include "sqliteproductsrm.h"
#include "sqliteproductdefaultsrm.h"
#include "sqliteproductstatisticsrm.h"
#include "sqliteproductdetailedstatisticsrm.h"
#include "sqliteproducttextsrm.h"
#include "sqliteproducttextcfgsrm.h"
#include "sqliterecallsrm.h"
#include "sqliteromlabelsrm.h"
#include "sqlitescalesrm.h"
#include "sqlitesoftkeysrm.h"
#include "sqlitesoftkeyfeaturesrm.h"
#include "sqlitesoftkeygroupsrm.h"
#include "sqlitesoftkeystylesrm.h"
#include "sqlitespecialprogramsrm.h"
#include "sqlitestreaminggraphicsrm.h"
#include "sqlitesynchronizationchangesrm.h"
#include "sqlitetransactionsrm.h"
#include "sqlitetransactiontypesrm.h"
#include "sqlitewrappererrorhistoryrm.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;
	class SqliteRecordManager;
	class SqliteFactory;


	// The following class and class template are currently outside the SqliteFactory class
	// because the VxWorks C++ compiler (GNU compiler) was complaining about it.
	class FactoryFunctorBase
	{
		public:
			virtual ~FactoryFunctorBase() {}
			virtual SqliteRecordManager* operator()() = 0;
	};


	template <class SqliteEntityRM>
	class FactoryFunctor : public FactoryFunctorBase
	{
		public:
			typedef SqliteEntityRM* (SqliteFactory::* factoryMethod_type)();
			FactoryFunctor(SqliteFactory* f, factoryMethod_type m)
					: factory_(f), factoryMethod_(m) {}
			SqliteRecordManager* operator()() { return (factory_->*factoryMethod_)(); }
		private:
			SqliteFactory* factory_;
			factoryMethod_type factoryMethod_;
	};


	class SqliteFactory : public DBFactoryI
	{
		public:

			// --- Types and subtypes ---
			typedef SqliteRecordManager* (SqliteFactory::* factoryMethod_type)();
			// The classes defined outside SqliteFactory, FactoryFunctorBase and FactoryFunctor
			// were originally here.
			typedef std::vector<FactoryFunctorBase*> factoryMethodContainer_type;


			// --- Constructors and destructor ---
			SqliteFactory(const SqliteDatabase& sdb, const util_env::Properties& props, DBFactoryI* f = 0);
			~SqliteFactory();


			// --- Accessor methods ---
			const factoryMethodContainer_type& getFactoryMethods() const { return factoryMethods_; }


			// --- Processing methods ---
			// Methods from DBFactoryI

			SqliteBatchesRM*                                createBatches();
			SqliteBarcodeScannersRM*                        createBarcodeScanners();
			SqliteCategoriesRM*                             createCategories();
			SqliteCategoryStatisticsRM*                     createCategoryStatistics();
			SqliteCategoryDetailedStatisticsRM*             createCategoryDetailedStatistics();
			SqliteClassesRM*                                createClasses();
			SqliteConfigurationsRM*                         createConfigurations();
			SqliteCoolCategoriesRM*                         createCoolCategories();
			SqliteCoolCategoryCountriesRM*                  createCoolCategoryCountries();
			SqliteCoolCategoryPreTextsRM*                   createCoolCategoryPreTexts();
			SqliteCoolCategoryProductionStepsRM*            createCoolCategoryProductionSteps();
			SqliteCoolCountriesRM*                          createCoolCountries();
			SqliteCoolListsRM*                              createCoolLists();
			SqliteCoolListTextsRM*                          createCoolListTexts();
			SqliteCoolPreTextsRM*                           createCoolPreTexts();
			SqliteCoolProductionStepsRM*                    createCoolProductionSteps();
			SqliteCoolTextsRM*                              createCoolTexts();
			SqliteCuttingTestsRM*                           createCuttingTests();
			SqliteDeptsRM*                                  createDepts();
			SqliteDevicesRM*                                createDevices();
			SqliteEmailAlertsRM*                            createEmailAlerts();
			SqliteExpirationLabelsRM*                       createExpirationLabels();
			SqliteFlashFilesRM*                             createFlashFiles();
			SqliteKnifeTimeHistoryRM*                       createWrapperKnife();						
			SqliteLabelsRM*                                 createLabels();
			SqliteLabelTextsRM*                             createLabelTexts();
			SqliteNetworkErrorsRM*                          createNetworkErrors();
			SqliteNutritionsRM*                             createNutritions();
			SqliteOperatorsRM*                              createOperators();
			SqliteOperatorNotesRM*                          createOperatorNotes();
			SqlitePresentationMethodsRM*                    createPresentationMethods();
			SqlitePresentationMethodStatisticsRM*           createPresentationMethodStatistics();
			SqlitePresentationMethodDetailedStatisticsRM*   createPresentationMethodDetailedStatistics();
			SqliteProductsRM*                               createProducts();
			SqliteProductDefaultsRM*                        createProductDefaults();
			SqliteProductStatisticsRM*                      createProductStatistics();
			SqliteProductDetailedStatisticsRM*              createProductDetailedStatistics();
			SqliteProductTextsRM*                           createProductTexts();
			SqliteProductTextCfgsRM*                        createProductTextCfgs();
			SqliteRecallsRM*                                createRecalls();
			SqliteROMLabelsRM*                              createROMLabels();
			SqliteScalesRM*                                 createScales();
			SqliteSoftKeysRM*                               createSoftKeys();
			SqliteSoftKeyFeaturesRM*                        createSoftKeyFeatures();
			SqliteSoftKeyGroupsRM*                          createSoftKeyGroups();
			SqliteSoftKeyStylesRM*                          createSoftKeyStyles();
			SqliteSpecialProgramsRM*                        createSpecialPrograms();
			SqliteStreamingGraphicsRM*                      createStreamingGraphics();
			SqliteSynchronizationChangesRM*                 createSynchronizationChanges();
			SqliteTransactionsRM*                           createTransactions();
			SqliteTransactionTypesRM*                       createTransactionTypes();
			SqliteWrapperErrorHistoryRM*                    createWrapperErrorHistory();									


		private:

			const SqliteDatabase& sdb_;
			const util_env::Properties& config_;
			factoryMethodContainer_type factoryMethods_;
			DBFactoryI* decorated_;

			void createFactoryMethods();
			void deleteFactoryMethods();

			// Do not allow the default copy constructor.
			SqliteFactory(const SqliteFactory& f);
			// Do not allow the default assignment operator.
			SqliteFactory operator=(const SqliteFactory& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEFACTORY_H
