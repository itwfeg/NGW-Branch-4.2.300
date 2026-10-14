#ifndef HFCMANAGERCONTAINER_H
#define HFCMANAGERCONTAINER_H



#include <string>
#include <vector>
#include <map>


#include "DBAccess.h"
#include "Product.h"

#include "util/thread/mutex.h"
#include "CSmartPtr.h"



namespace edb
{
	class DBException;
}


class DataManager;

class CategoryGraphicManager;
class CategoryManager;
class ClassManager;
class ConfigurationManager;
class COOLCategoryCountryManager;
class COOLCategoryManager;
class COOLCategoryPretextManager;
class COOLCategoryProdStepManager;
class COOLCountryManager;
class COOLListTextManager;
class COOLManager;
class COOLPretextManager;
class COOLProductionStepManager;
class COOLShortlistManager;
class COOLTextManager;
class DeptManager;
class DeviceManager;
class EmailAlertsManager;
class ExpirationLabelManager;
class FlashFileManager;
class GenericLabelTextManager;
class GraphicManager;
class LabelGraphicManager;
class LabelTypeManager;
class NutrifactsManager;
class OperatorManager;
class OperatorNoteManager;
class PeripheralManager;
class PlanogramGraphicManager;
class ProductDefaultManager;
class ProductGraphicManager;
class ProductManager;
class ProductTextManager;
class ProductTextCfgManager;
class BarcodeScannerManager;
class RecallManager;
class SoftkeyFeatureManager;
class SoftkeyGraphicManager;
class SoftkeyGroupManager;
class SoftkeyManager;
class SoftkeyStyleManager;
class SpecialsGraphicManager;
class SpecialsManager;
class StreamingGraphicManager;
class EvolWrapperKnifeHistoryManager;
class EvolWrapperErrorHistoryManager;
class CutTestManager;
class CutTestTotalManager;

class EvolMessage;


namespace hfc
{


	class BatchManager;
	class ClassTotalManager;
	class COOLTotalManager;
	class HourlyTotalManager;
	class OperatorChangeManager;
	class OperatorTotalManager;
	class ProductTotalManager;
	class ScaleManager;
	class SynchronizationChangeManager;
	class TransactionManager;
	class TransactionTypeManager;


	class HFCManagerContainer : public DBAccess
	{
		public:
			/* --- Types and subtypes --- */

			// This originally came from the edb::DBManager, in dbmanager.h
			// I had to add the HFC Managers that either handle a collection of database files,
			// or that don't have a corresponding database file.
			enum HFCRecordManagerType 
			{
				InvalidType,

				Batches,								/*  0 */
				Categories,
				CategoryDetailedStatistics,
				CategoryStatistics,
				Classes,
				ClassTotals,							/*  5 */
				Configurations,
				CoolCategories,
				CoolCategoryCountries,
				CoolCategoryPreTexts,
				CoolCategoryProductionSteps,			/* 10 */
				CoolCountries,
				CoolLists,
				CoolListTexts,
				CoolPreTexts,
				CoolProductionSteps,					/* 15 */
				CoolTexts,
				CoolTotals,
				CutTests,
				CutTestTotals,
				Devices,								/* 20 */
				EmailAlerts,
				ExpirationLabels,
				FlashFiles,
				HourlyTotals,
				Labels,									/* 25 */
				LabelTexts,
				Marquees,
				Messages,
				NetworkErrors,							// will go away
				Notes,									/* 30 */
				Nutritions,
				Operators,
				OperatorChanges,
				OperatorNotes,
				OperatorTotals,							/* 35 */
				PresentationMethods,
				PresentationMethodStatistics,
				PresentationMethodDetailedStatistics,
				Products,
				ProductDefaults,						/* 40 */
				ProductDetailedStatistics,
				ProductStatistics,
				ProductTotals,
				ROMLabels,
				Scales,									/* 45 */
				SoftKeys,
				SoftKeyFeatures,
				SoftKeyGroups,
				SoftKeyStyles,
				SpecialPrograms,						/* 50 */
				Status,
				StreamingGraphics,
				SynchronizationChanges,
				Texts,
				Transactions,							/* 55 */
				TransactionTypes,
				WrapperKnifeTime,
				WrapperErrorHistory,

				ProductText5,
				ProductText6,
				ProductText7,
				ProductText8,
				ProductText9,
				Recalls,

				ProductTextCfgs,
				BarcodeScanners,			// jhc ss elev: Is this okay here?

				Depts,

				LastDBRecordManager,		// renamed to indicate the end of DBManager's definitions

				// The new ones not in the DBManager's enum

				COOL,
				Totals,   // TODO:SJJC It seems no-one is using this so it should be removed.
				Graphics,
				LabelGraphics,
				CategoryGraphics,
				ProductGraphics,
				SoftkeyGraphics,
				SpecialsGraphics,
				Peripherals,

				PlanogramGraphics,

				LastRecordManager
			};

			typedef std::vector<HFCRecordManagerType> recordTypeContainer_type;
			typedef recordTypeContainer_type::iterator recordTypeContainerIterator_type;
			typedef std::vector<DataManager*> managerContainer_type;
			typedef managerContainer_type::iterator managerContainerIterator_type;
			typedef std::map<HFCRecordManagerType, bool> syncList_type;
			typedef std::map<HFCRecordManagerType, bool>::iterator syncListIterator_type;


			/* --- Constructors and destructor --- */
			HFCManagerContainer();
			virtual ~HFCManagerContainer();


			/* --- Instance function members --- */
			BatchManager *                      getBatchManager()                      { return                        (BatchManager*) (managers_[HFCManagerContainer::Batches]                     ? managers_[HFCManagerContainer::Batches]                     : useDataManager(HFCManagerContainer::Batches)); }
			BarcodeScannerManager *             getBarcodeScannerManager()             { return               (BarcodeScannerManager*) (managers_[HFCManagerContainer::BarcodeScanners]             ? managers_[HFCManagerContainer::BarcodeScanners]             : useDataManager(HFCManagerContainer::BarcodeScanners)); }
			CategoryGraphicManager *            getCategoryGraphicManager()            { return              (CategoryGraphicManager*) (managers_[HFCManagerContainer::CategoryGraphics]            ? managers_[HFCManagerContainer::CategoryGraphics]            : useDataManager(HFCManagerContainer::CategoryGraphics)); }
			CategoryManager *                   getCategoryManager()                   { return                     (CategoryManager*) (managers_[HFCManagerContainer::Categories]                  ? managers_[HFCManagerContainer::Categories]                  : useDataManager(HFCManagerContainer::Categories)); }
			ClassManager *                      getClassManager()                      { return                        (ClassManager*) (managers_[HFCManagerContainer::Classes]                     ? managers_[HFCManagerContainer::Classes]                     : useDataManager(HFCManagerContainer::Classes)); }
			ClassTotalManager *                 getClassTotalManager()                 { return                   (ClassTotalManager*) (managers_[HFCManagerContainer::ClassTotals]                 ? managers_[HFCManagerContainer::ClassTotals]                 : useDataManager(HFCManagerContainer::ClassTotals)); }
			ConfigurationManager *              getConfigurationManager()              { return                (ConfigurationManager*) (managers_[HFCManagerContainer::Configurations]              ? managers_[HFCManagerContainer::Configurations]              : useDataManager(HFCManagerContainer::Configurations)); }
			COOLCategoryCountryManager *        getCOOLCategoryCountryManager()        { return          (COOLCategoryCountryManager*) (managers_[HFCManagerContainer::CoolCategoryCountries]       ? managers_[HFCManagerContainer::CoolCategoryCountries]       : useDataManager(HFCManagerContainer::CoolCategoryCountries)); }
			COOLCategoryManager *               getCOOLCategoryManager()               { return                 (COOLCategoryManager*) (managers_[HFCManagerContainer::CoolCategories]              ? managers_[HFCManagerContainer::CoolCategories]              : useDataManager(HFCManagerContainer::CoolCategories)); }
			COOLCategoryPretextManager *        getCOOLCategoryPretextManager()        { return          (COOLCategoryPretextManager*) (managers_[HFCManagerContainer::CoolCategoryPreTexts]        ? managers_[HFCManagerContainer::CoolCategoryPreTexts]        : useDataManager(HFCManagerContainer::CoolCategoryPreTexts)); }
			COOLCategoryProdStepManager *       getCOOLCategoryProdStepManager()       { return         (COOLCategoryProdStepManager*) (managers_[HFCManagerContainer::CoolCategoryProductionSteps] ? managers_[HFCManagerContainer::CoolCategoryProductionSteps] : useDataManager(HFCManagerContainer::CoolCategoryProductionSteps)); }
			COOLCountryManager *                getCOOLCountryManager()                { return                  (COOLCountryManager*) (managers_[HFCManagerContainer::CoolCountries]               ? managers_[HFCManagerContainer::CoolCountries]               : useDataManager(HFCManagerContainer::CoolCountries)); }
			COOLListTextManager *               getCOOLListTextManager()               { return                 (COOLListTextManager*) (managers_[HFCManagerContainer::CoolListTexts]               ? managers_[HFCManagerContainer::CoolListTexts]               : useDataManager(HFCManagerContainer::CoolListTexts)); }
			COOLManager *                       getCOOLManager()                       { return                         (COOLManager*) (managers_[HFCManagerContainer::COOL]                        ? managers_[HFCManagerContainer::COOL]                        : useDataManager(HFCManagerContainer::COOL)); }
			COOLPretextManager *                getCOOLPretextManager()                { return                  (COOLPretextManager*) (managers_[HFCManagerContainer::CoolPreTexts]                ? managers_[HFCManagerContainer::CoolPreTexts]                : useDataManager(HFCManagerContainer::CoolPreTexts)); }
			COOLProductionStepManager *         getCOOLProductionStepManager()         { return           (COOLProductionStepManager*) (managers_[HFCManagerContainer::CoolProductionSteps]         ? managers_[HFCManagerContainer::CoolProductionSteps]         : useDataManager(HFCManagerContainer::CoolProductionSteps)); }
			COOLShortlistManager *              getCOOLShortlistManager()              { return                (COOLShortlistManager*) (managers_[HFCManagerContainer::CoolLists]                   ? managers_[HFCManagerContainer::CoolLists]                   : useDataManager(HFCManagerContainer::CoolLists)); }
			COOLTextManager *                   getCOOLTextManager()                   { return                     (COOLTextManager*) (managers_[HFCManagerContainer::CoolTexts]                   ? managers_[HFCManagerContainer::CoolTexts]                   : useDataManager(HFCManagerContainer::CoolTexts)); }
			COOLTotalManager *                  getCOOLTotalManager()                  { return                    (COOLTotalManager*) (managers_[HFCManagerContainer::CoolTotals]                  ? managers_[HFCManagerContainer::CoolTotals]                  : useDataManager(HFCManagerContainer::CoolTotals)); }
			DeptManager *                       getDeptManager()                       { return                         (DeptManager*) (managers_[HFCManagerContainer::Depts]                       ? managers_[HFCManagerContainer::Depts]                       : useDataManager(HFCManagerContainer::Depts)); }
			DeviceManager *                     getDeviceManager()                     { return                       (DeviceManager*) (managers_[HFCManagerContainer::Devices]                     ? managers_[HFCManagerContainer::Devices]                     : useDataManager(HFCManagerContainer::Devices)); }
			EmailAlertsManager *                getEmailAlertsManager()                { return                  (EmailAlertsManager*) (managers_[HFCManagerContainer::EmailAlerts]                 ? managers_[HFCManagerContainer::EmailAlerts]                 : useDataManager(HFCManagerContainer::EmailAlerts)); }
			ProductTextManager *                getExpandedTextManager()               { return getProductTextManager(psttExpandedText); }
			ExpirationLabelManager *            getExpirationLabelManager()            { return              (ExpirationLabelManager*) (managers_[HFCManagerContainer::ExpirationLabels]            ? managers_[HFCManagerContainer::ExpirationLabels]            : useDataManager(HFCManagerContainer::ExpirationLabels)); }
			FlashFileManager *                  getFlashFileManager()                  { return                    (FlashFileManager*) (managers_[HFCManagerContainer::FlashFiles]                  ? managers_[HFCManagerContainer::FlashFiles]                  : useDataManager(HFCManagerContainer::FlashFiles)); }
			GenericLabelTextManager *           getGenericLabelTextManager()           { return             (GenericLabelTextManager*) (managers_[HFCManagerContainer::LabelTexts]                  ? managers_[HFCManagerContainer::LabelTexts]                  : useDataManager(HFCManagerContainer::LabelTexts)); }
			GraphicManager *                    getGraphicManager()                    { return                      (GraphicManager*) (managers_[HFCManagerContainer::Graphics]                    ? managers_[HFCManagerContainer::Graphics]                    : useDataManager(HFCManagerContainer::Graphics)); }
			HourlyTotalManager *                getHourlyTotalManager()                { return                  (HourlyTotalManager*) (managers_[HFCManagerContainer::HourlyTotals]                ? managers_[HFCManagerContainer::HourlyTotals]                : useDataManager(HFCManagerContainer::HourlyTotals)); }
			LabelGraphicManager *               getLabelGraphicManager()               { return                 (LabelGraphicManager*) (managers_[HFCManagerContainer::LabelGraphics]               ? managers_[HFCManagerContainer::LabelGraphics]               : useDataManager(HFCManagerContainer::LabelGraphics)); }
			LabelTypeManager *                  getLabelTypeManager()                  { return                    (LabelTypeManager*) (managers_[HFCManagerContainer::Labels]                      ? managers_[HFCManagerContainer::Labels]                      : useDataManager(HFCManagerContainer::Labels)); }
			ProductTextManager *                getMarqueeTextManager()                { return getProductTextManager(psttMarqueeText); }
			NutrifactsManager *                 getNutrifactsManager()                 { return                   (NutrifactsManager*) (managers_[HFCManagerContainer::Nutritions]                  ? managers_[HFCManagerContainer::Nutritions]                  : useDataManager(HFCManagerContainer::Nutritions)); }
			OperatorChangeManager *             getOperatorChangeManager()             { return               (OperatorChangeManager*) (managers_[HFCManagerContainer::OperatorChanges]             ? managers_[HFCManagerContainer::OperatorChanges]             : useDataManager(HFCManagerContainer::OperatorChanges)); }
			OperatorManager *                   getOperatorManager()                   { return                     (OperatorManager*) (managers_[HFCManagerContainer::Operators]                   ? managers_[HFCManagerContainer::Operators]                   : useDataManager(HFCManagerContainer::Operators)); }
			OperatorNoteManager *               getOperatorNoteManager()               { return                 (OperatorNoteManager*) (managers_[HFCManagerContainer::OperatorNotes]               ? managers_[HFCManagerContainer::OperatorNotes]               : useDataManager(HFCManagerContainer::OperatorNotes)); }
			OperatorTotalManager *              getOperatorTotalManager()              { return                (OperatorTotalManager*) (managers_[HFCManagerContainer::OperatorTotals]              ? managers_[HFCManagerContainer::OperatorTotals]              : useDataManager(HFCManagerContainer::OperatorTotals)); }
			PeripheralManager *                 getPeripheralManager()                 { return                   (PeripheralManager*) (managers_[HFCManagerContainer::Peripherals]                 ? managers_[HFCManagerContainer::Peripherals]                 : useDataManager(HFCManagerContainer::Peripherals)); }
			ProductDefaultManager *             getProductDefaultManager()             { return               (ProductDefaultManager*) (managers_[HFCManagerContainer::ProductDefaults]             ? managers_[HFCManagerContainer::ProductDefaults]             : useDataManager(HFCManagerContainer::ProductDefaults)); }
			ProductGraphicManager *             getProductGraphicManager()             { return               (ProductGraphicManager*) (managers_[HFCManagerContainer::ProductGraphics]             ? managers_[HFCManagerContainer::ProductGraphics]             : useDataManager(HFCManagerContainer::ProductGraphics)); }
			ProductManager *                    getProductManager()                    { return                      (ProductManager*) (managers_[HFCManagerContainer::Products]                    ? managers_[HFCManagerContainer::Products]                    : useDataManager(HFCManagerContainer::Products)); }
			ProductTextManager *                getProductNoteManager()                { return getProductTextManager(psttProductNote); }
			ProductTextCfgManager *             getProductTextCfgManager()             { return               (ProductTextCfgManager*) (managers_[HFCManagerContainer::ProductTextCfgs]             ? managers_[HFCManagerContainer::ProductTextCfgs]             : useDataManager(HFCManagerContainer::ProductTextCfgs)); }
			ProductTotalManager *               getProductTotalManager()               { return                 (ProductTotalManager*) (managers_[HFCManagerContainer::ProductTotals]               ? managers_[HFCManagerContainer::ProductTotals]               : useDataManager(HFCManagerContainer::ProductTotals)); }
			ScaleManager *                      getScaleManager()                      { return                        (ScaleManager*) (managers_[HFCManagerContainer::Scales]                      ? managers_[HFCManagerContainer::Scales]                      : useDataManager(HFCManagerContainer::Scales)); }
			SoftkeyFeatureManager *             getSoftkeyFeatureManager()             { return               (SoftkeyFeatureManager*) (managers_[HFCManagerContainer::SoftKeyFeatures]             ? managers_[HFCManagerContainer::SoftKeyFeatures]             : useDataManager(HFCManagerContainer::SoftKeyFeatures)); }
			SoftkeyGraphicManager *             getSoftkeyGraphicManager()             { return               (SoftkeyGraphicManager*) (managers_[HFCManagerContainer::SoftkeyGraphics]             ? managers_[HFCManagerContainer::SoftkeyGraphics]             : useDataManager(HFCManagerContainer::SoftkeyGraphics)); }
			SoftkeyGroupManager *               getSoftkeyGroupManager()               { return                 (SoftkeyGroupManager*) (managers_[HFCManagerContainer::SoftKeyGroups]               ? managers_[HFCManagerContainer::SoftKeyGroups]               : useDataManager(HFCManagerContainer::SoftKeyGroups)); }
			SoftkeyManager *                    getSoftkeyManager()                    { return                      (SoftkeyManager*) (managers_[HFCManagerContainer::SoftKeys]                    ? managers_[HFCManagerContainer::SoftKeys]                    : useDataManager(HFCManagerContainer::SoftKeys)); }
			SoftkeyStyleManager *               getSoftkeyStyleManager()               { return                 (SoftkeyStyleManager*) (managers_[HFCManagerContainer::SoftKeyStyles]               ? managers_[HFCManagerContainer::SoftKeyStyles]               : useDataManager(HFCManagerContainer::SoftKeyStyles)); }
			ProductTextManager *                getSpecialMessageManager()             { return getProductTextManager(psttSpecialMessage); }
			SpecialsGraphicManager *            getSpecialsGraphicManager()            { return              (SpecialsGraphicManager*) (managers_[HFCManagerContainer::SpecialsGraphics]            ? managers_[HFCManagerContainer::SpecialsGraphics]            : useDataManager(HFCManagerContainer::SpecialsGraphics)); }
			SpecialsManager *                   getSpecialsManager()                   { return                     (SpecialsManager*) (managers_[HFCManagerContainer::SpecialPrograms]             ? managers_[HFCManagerContainer::SpecialPrograms]             : useDataManager(HFCManagerContainer::SpecialPrograms)); }
			StreamingGraphicManager *           getStreamingGraphicManager()           { return             (StreamingGraphicManager*) (managers_[HFCManagerContainer::StreamingGraphics]           ? managers_[HFCManagerContainer::StreamingGraphics]           : useDataManager(HFCManagerContainer::StreamingGraphics)); }
			SynchronizationChangeManager *      getSynchronizationChangeManager()      { return        (SynchronizationChangeManager*) (managers_[HFCManagerContainer::SynchronizationChanges]      ? managers_[HFCManagerContainer::SynchronizationChanges]      : useDataManager(HFCManagerContainer::SynchronizationChanges)); }
			TransactionManager *                getTransactionManager()                { return                  (TransactionManager*) (managers_[HFCManagerContainer::Transactions]                ? managers_[HFCManagerContainer::Transactions]                : useDataManager(HFCManagerContainer::Transactions)); }
			TransactionTypeManager *            getTransactionTypeManager()            { return              (TransactionTypeManager*) (managers_[HFCManagerContainer::TransactionTypes]            ? managers_[HFCManagerContainer::TransactionTypes]            : useDataManager(HFCManagerContainer::TransactionTypes)); }
			EvolWrapperKnifeHistoryManager *    getEvolWrapperKnifeHistoryManager()    { return      (EvolWrapperKnifeHistoryManager*) (managers_[HFCManagerContainer::WrapperKnifeTime]            ? managers_[HFCManagerContainer::WrapperKnifeTime]            : useDataManager(HFCManagerContainer::WrapperKnifeTime)); }
			EvolWrapperErrorHistoryManager *    getEvolWrapperErrorHistoryManager()    { return      (EvolWrapperErrorHistoryManager*) (managers_[HFCManagerContainer::WrapperErrorHistory]         ? managers_[HFCManagerContainer::WrapperErrorHistory]         : useDataManager(HFCManagerContainer::WrapperErrorHistory)); }
			CutTestManager *                    getCutTestManager()                    { return                      (CutTestManager*) (managers_[HFCManagerContainer::CutTests]                    ? managers_[HFCManagerContainer::CutTests]                    : useDataManager(HFCManagerContainer::CutTests)); }
			CutTestTotalManager *               getCutTestTotalManager()               { return                 (CutTestTotalManager*) (managers_[HFCManagerContainer::CutTestTotals]               ? managers_[HFCManagerContainer::CutTestTotals]               : useDataManager(HFCManagerContainer::CutTestTotals)); }
			PlanogramGraphicManager *           getPlanogramGraphicManager()           { return             (PlanogramGraphicManager*) (managers_[HFCManagerContainer::PlanogramGraphics]           ? managers_[HFCManagerContainer::PlanogramGraphics]           : useDataManager(HFCManagerContainer::PlanogramGraphics)); }
			RecallManager *                     getRecallManager()                     { return                       (RecallManager*) (managers_[HFCManagerContainer::Recalls]                     ? managers_[HFCManagerContainer::Recalls]                     : useDataManager(HFCManagerContainer::Recalls)); }

			DataManager *                       getDataManager(recordTypeContainerIterator_type iter) { return managers_[*iter] ? managers_[*iter] : useDataManager(*iter); }
			DataManager *                       getDataManager(syncListIterator_type iter) { return managers_[iter->first] ? managers_[iter->first] : useDataManager(iter->first); }
			DataManager *                       getExistingDataManager(syncListIterator_type iter) { return managers_[iter->first]; }


			ProductTextManager *getProductTextManager(ProdSupportTextType type);

//			ProductTextCfgManager *             getProductTextCfgManager()             { return               (ProductTextCfgManager*) (managers_[HFCManagerContainer::ProductTextCfgs]             ? managers_[HFCManagerContainer::ProductTextCfgs]             : useDataManager(HFCManagerContainer::ProductTextCfgs)); }

			void init();
			void destroyQueriesAndIters();
			virtual void initData();
			virtual void uninitData();


			syncListIterator_type beginSynchronizedDataManager();
			syncListIterator_type nextSynchronizedDataManager(syncListIterator_type iter);
			syncListIterator_type endSynchronizedDataManager() { return _synchronizedTypes_.end(); }

			recordTypeContainerIterator_type beginDeleteAllFilesDataManager() { return _deleteAllFilesTypes_.begin(); }
			recordTypeContainerIterator_type endDeleteAllFilesDataManager() { return _deleteAllFilesTypes_.end(); }

			recordTypeContainerIterator_type beginDeleteAllFilesForDeptDataManager() { return _deleteAllFilesForDeptTypes_.begin(); }
			recordTypeContainerIterator_type endDeleteAllFilesForDeptDataManager() { return _deleteAllFilesForDeptTypes_.end(); }

			bool isSynchronized(HFCManagerContainer::HFCRecordManagerType type);

			virtual void dispatchMessage(EvolMessage *) {}


			static void updateSyncList();


		protected:
			/* --- Instance data members --- */


			/* --- Instance function members --- */
			DataManager * useDataManager(HFCManagerContainer::HFCRecordManagerType);

			void showDatabaseException(const std::string & module, const edb::DBException & dbe);


		private:
			/* --- Instance data members --- */
			managerContainer_type managers_;
//			edb::DBManager* dbManager_;


			/* --- Instance class members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif


			/* --- Instance function members --- */


			/* --- Class data members --- */
			static bool _typeContainersInitialized_;

			static syncList_type _synchronizedTypes_;
			static recordTypeContainer_type _deleteAllFilesTypes_;
			static recordTypeContainer_type _deleteAllFilesForDeptTypes_;

			static util_thread::Mutex<> _synchronizedTypesMutex_;

			static void initializeTypeContainers();
	};


}



#endif /* HFCMANAGERCONTAINER_H */
