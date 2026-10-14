#include "HFCManagerContainer.h"

#include "DBManager.h"
#include "DBException.h"

#include "CategoryGraphicManager.h"
#include "CategoryManager.h"
#include "ClassManager.h"
#include "ClassTotalManager.h"
#include "ConfigurationManager.h"
#include "COOLManager.h"
#include "COOLCategoryManager.h"
#include "COOLCategoryCountryManager.h"
#include "COOLCategoryPretextManager.h"
#include "COOLCategoryProdStepManager.h"
#include "COOLCountryManager.h"
#include "COOLListTextManager.h"
#include "COOLPretextManager.h"
#include "COOLProductionStepManager.h"
#include "COOLShortlistManager.h"
#include "COOLTextManager.h"
#include "COOLTotalManager.h"
#include "DeptManager.h"
#include "DeviceManager.h"
#include "EmailAlertsManager.h"
#include "ExpirationLabelManager.h"
#include "FlashFileManager.h"
#include "GenericLabelTextManager.h"
#include "GraphicManager.h"
#include "HourlyTotalManager.h"
#include "LabelGraphicManager.h"
#include "LabelTypeManager.h"
#include "NutrifactsManager.h"
#include "OperatorManager.h"
#include "OperatorChangeManager.h"
#include "OperatorNoteManager.h"
#include "OperatorTotalManager.h"
#include "PeripheralManager.h"
#include "PlanogramGraphicManager.h"
#include "ProductManager.h"
#include "ProductDefaultManager.h"
#include "ProductGraphicManager.h"
#include "ProductTextCfgManager.h"
#include "BarcodeScannerManager.h"
#include "ProductTextManager.h"
#include "ProductTotalManager.h"
#include "SoftkeyManager.h"
#include "SoftkeyFeatureManager.h"
#include "SoftkeyGraphicManager.h"
#include "SoftkeyGroupManager.h"
#include "SoftkeyStyleManager.h"
#include "SpecialsGraphicManager.h"
#include "SpecialsManager.h"
#include "StreamingGraphicManager.h"
#include "TransactionManager.h"
#include "TransactionTypeManager.h"

#include "ScaleManager.h"
#include "BatchManager.h"
#include "SynchronizationChangeManager.h"
#include "EvolWrapperKnifeHistoryManager.h"
#include "EvolWrapperErrorHistoryManager.h"
#include "CutTestManager.h"
#include "CutTestTotalManager.h"

#include "RecallManager.h"

#include "DBQuery.h"
#include "DBCoolCategoriesRM.h"
#include "DBCoolCountriesRM.h"
#include "DBCoolProductionStepsRM.h"
#include "DBCoolPreTextsRM.h"
#include "DBCoolCategoryCountriesRM.h"
#include "DBCoolCategoryPreTextsRM.h"
#include "DBCoolCategoryProductionStepsRM.h"
#include "DBCoolTextsRM.h"
#include "DBCoolListsRM.h"
#include "DBCoolListTextsRM.h"
#include "DBExpirationLabelsRM.h"
#include "DBProductsRM.h"
#include "DBLabelsRM.h"
#include "DBROMLabelsRM.h"
#include "DBLabelTextsRM.h"
#include "DBProductTextsRM.h"
#include "DBProductTextCfgsRM.h"
#include "DBBarcodeScannersRM.h"
#include "DBClassesRM.h"
#include "DBNutritionsRM.h"
#include "DBTransactionTypesRM.h"
#include "DBOperatorsRM.h"
#include "DBOperatorNotesRM.h"
#include "DBSoftKeysRM.h"
#include "DBSoftKeyGroupsRM.h"
#include "DBSoftKeyFeaturesRM.h"
#include "DBSoftKeyStylesRM.h"
#include "DBSpecialProgramsRM.h"
#include "DBStreamingGraphicsRM.h"

#include "DBRecallsRM.h"

#include "DBDeptsRM.h"

#include "Configuration.h"

#include "Integer.h"

#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	bool HFCManagerContainer::_typeContainersInitialized_ = false;

	HFCManagerContainer::syncList_type HFCManagerContainer::_synchronizedTypes_;
	HFCManagerContainer::recordTypeContainer_type HFCManagerContainer::_deleteAllFilesTypes_;
	HFCManagerContainer::recordTypeContainer_type HFCManagerContainer::_deleteAllFilesForDeptTypes_;

	util_thread::Mutex<> HFCManagerContainer::_synchronizedTypesMutex_;

	#ifdef USE_ACCOUNTANT
		int HFCManagerContainer::_dataType_ = 0;
		string HFCManagerContainer::_dataTypeName_("hfc::HFCManagerContainer");
	#endif

		
		
	HFCManagerContainer::HFCManagerContainer()
 			: /* productManager_(0), scaleManager_(0), batchManager_(0), synchronizationChangeManager_(0), */
			managers_(HFCManagerContainer::LastRecordManager, NULL)//, dbManager_(0)
 	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

			
		if ( !_typeContainersInitialized_ )
		{
			util_thread::Lock< util_thread::Mutex<> > lock(_synchronizedTypesMutex_);

			if ( !_typeContainersInitialized_ )
				initializeTypeContainers();
		}


		// This code is unnecessary since the container is initialized to the default value
//		for ( int i = 0; i < HFCManagerContainer::LastRecordManager; ++i )
//			managers_[i] = 0;


	//	dbManager_ = HFCManagerContainer::getInstance();
//		initDB();
	}


	HFCManagerContainer::~HFCManagerContainer()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		init();

	//	if ( dbManager_ )
	//		HFCManagerContainer::destroyInstance(dbManager_);
	}


	void HFCManagerContainer::init()
	{
		Debug::trace("HFCManagerContainer", "init() - begin");

		for ( int i = 0; i < HFCManagerContainer::LastRecordManager; ++i )
		{
			if ( managers_[i] )
			{
				delete managers_[i];
				managers_[i] = 0;
			}
		}

		Debug::trace("HFCManagerContainer", "init() - end");
	}

	ProductTextManager *HFCManagerContainer::getProductTextManager(ProdSupportTextType type)
	{
		switch (type)
		{
			case psttExpandedText:
				return (ProductTextManager*) (managers_[HFCManagerContainer::Texts]        ? managers_[HFCManagerContainer::Texts]        : useDataManager(HFCManagerContainer::Texts));         break;
			case psttSpecialMessage:
				return (ProductTextManager*) (managers_[HFCManagerContainer::Messages]     ? managers_[HFCManagerContainer::Messages]     : useDataManager(HFCManagerContainer::Messages));      break;
			case psttProductNote:
				return (ProductTextManager*) (managers_[HFCManagerContainer::Notes]        ? managers_[HFCManagerContainer::Notes]        : useDataManager(HFCManagerContainer::Notes));         break;
			case psttMarqueeText:
				return (ProductTextManager*) (managers_[HFCManagerContainer::Marquees]     ? managers_[HFCManagerContainer::Marquees]     : useDataManager(HFCManagerContainer::Marquees));      break;
			case psttProductText5:
				return (ProductTextManager*) (managers_[HFCManagerContainer::ProductText5] ? managers_[HFCManagerContainer::ProductText5] : useDataManager(HFCManagerContainer::ProductText5));  break;
			case psttProductText6:
				return (ProductTextManager*) (managers_[HFCManagerContainer::ProductText6] ? managers_[HFCManagerContainer::ProductText6] : useDataManager(HFCManagerContainer::ProductText6));  break;
			case psttProductText7:
				return (ProductTextManager*) (managers_[HFCManagerContainer::ProductText7] ? managers_[HFCManagerContainer::ProductText7] : useDataManager(HFCManagerContainer::ProductText7));  break;
			case psttProductText8:
				return (ProductTextManager*) (managers_[HFCManagerContainer::ProductText8] ? managers_[HFCManagerContainer::ProductText8] : useDataManager(HFCManagerContainer::ProductText8));  break;
			case psttProductText9:
				return (ProductTextManager*) (managers_[HFCManagerContainer::ProductText9] ? managers_[HFCManagerContainer::ProductText9] : useDataManager(HFCManagerContainer::ProductText9));  break;
			default:
				return 0;
		}
	}


	DataManager * HFCManagerContainer::useDataManager(HFCManagerContainer::HFCRecordManagerType type)
	{
		if ( !dbMgr )
			initDB();

		switch ( type )
		{
			case HFCManagerContainer::Batches:                                 managers_[HFCManagerContainer::Batches]                     = new BatchManager(this); break;
			case HFCManagerContainer::BarcodeScanners:                         managers_[HFCManagerContainer::BarcodeScanners]             = new BarcodeScannerManager(this); break;
			case HFCManagerContainer::Categories:                              managers_[HFCManagerContainer::Categories]                  = new CategoryManager(this); break;
			case HFCManagerContainer::CategoryDetailedStatistics:              break;
			case HFCManagerContainer::CategoryStatistics:                      break;
			case HFCManagerContainer::Classes:                                 managers_[HFCManagerContainer::Classes]                     = new ClassManager(this); break;
			case HFCManagerContainer::ClassTotals:                             managers_[HFCManagerContainer::ClassTotals]                 = new ClassTotalManager(this); break;
			case HFCManagerContainer::Configurations:                          managers_[HFCManagerContainer::Configurations]              = new ConfigurationManager(this); break;
			case HFCManagerContainer::CoolCategories:                          managers_[HFCManagerContainer::CoolCategories]              = new COOLCategoryManager(this); break;
			case HFCManagerContainer::CoolCategoryCountries:                   managers_[HFCManagerContainer::CoolCategoryCountries]       = new COOLCategoryCountryManager(this); break;
			case HFCManagerContainer::CoolCategoryPreTexts:                    managers_[HFCManagerContainer::CoolCategoryPreTexts]        = new COOLCategoryPretextManager(this); break;
			case HFCManagerContainer::CoolCategoryProductionSteps:             managers_[HFCManagerContainer::CoolCategoryProductionSteps] = new COOLCategoryProdStepManager(this); break;
			case HFCManagerContainer::CoolCountries:                           managers_[HFCManagerContainer::CoolCountries]               = new COOLCountryManager(this); break;
			case HFCManagerContainer::CoolLists:                               managers_[HFCManagerContainer::CoolLists]                   = new COOLShortlistManager(this); break;
			case HFCManagerContainer::CoolListTexts:                           managers_[HFCManagerContainer::CoolListTexts]               = new COOLListTextManager(this); break;
			case HFCManagerContainer::CoolPreTexts:                            managers_[HFCManagerContainer::CoolPreTexts]                = new COOLPretextManager(this); break;
			case HFCManagerContainer::CoolProductionSteps:                     managers_[HFCManagerContainer::CoolProductionSteps]         = new COOLProductionStepManager(this); break;
			case HFCManagerContainer::CoolTexts:                               managers_[HFCManagerContainer::CoolTexts]                   = new COOLTextManager(this); break;
			case HFCManagerContainer::CoolTotals:                              managers_[HFCManagerContainer::CoolTotals]                  = new COOLTotalManager(this); break;
			case HFCManagerContainer::CutTests:                                managers_[HFCManagerContainer::CutTests]                    = new CutTestManager(this); break;
			case HFCManagerContainer::CutTestTotals:                           managers_[HFCManagerContainer::CutTestTotals]               = new CutTestTotalManager(this); break;
			case HFCManagerContainer::Depts:                                   managers_[HFCManagerContainer::Depts]                       = new DeptManager(this); break;
			case HFCManagerContainer::Devices:                                 managers_[HFCManagerContainer::Devices]                     = new DeviceManager(this); break;
			case HFCManagerContainer::EmailAlerts:                             managers_[HFCManagerContainer::EmailAlerts]                 = new EmailAlertsManager(this); break;
			case HFCManagerContainer::ExpirationLabels:                        managers_[HFCManagerContainer::ExpirationLabels]            = new ExpirationLabelManager(this); break;
			case HFCManagerContainer::FlashFiles:                              managers_[HFCManagerContainer::FlashFiles]                  = new FlashFileManager(this); break;
			case HFCManagerContainer::HourlyTotals:                            managers_[HFCManagerContainer::HourlyTotals]                = new HourlyTotalManager(this); break;
			case HFCManagerContainer::Labels:                                  managers_[HFCManagerContainer::Labels]                      = new LabelTypeManager(this); break;
			case HFCManagerContainer::LabelTexts:                              managers_[HFCManagerContainer::LabelTexts]                  = new GenericLabelTextManager(this); break;
			case HFCManagerContainer::Marquees:                                managers_[HFCManagerContainer::Marquees]                    = new ProductTextManager(psttMarqueeText, this); break;
			case HFCManagerContainer::Messages:                                managers_[HFCManagerContainer::Messages]                    = new ProductTextManager(psttSpecialMessage, this); break;
			case HFCManagerContainer::NetworkErrors:                           break;
			case HFCManagerContainer::Notes:                                   managers_[HFCManagerContainer::Notes]                       = new ProductTextManager(psttProductNote, this); break;
			case HFCManagerContainer::Nutritions:                              managers_[HFCManagerContainer::Nutritions]                  = new NutrifactsManager(this); break;
			case HFCManagerContainer::Operators:                               managers_[HFCManagerContainer::Operators]                   = new OperatorManager(this); break;
			case HFCManagerContainer::OperatorChanges:                         managers_[HFCManagerContainer::OperatorChanges]             = new OperatorChangeManager(this); break;
			case HFCManagerContainer::OperatorNotes:                           managers_[HFCManagerContainer::OperatorNotes]               = new OperatorNoteManager(this); break;
			case HFCManagerContainer::OperatorTotals:                          managers_[HFCManagerContainer::OperatorTotals]              = new OperatorTotalManager(this); break;
			case HFCManagerContainer::PresentationMethods:                     break;
			case HFCManagerContainer::PresentationMethodStatistics:            break;
			case HFCManagerContainer::PresentationMethodDetailedStatistics:    break;
			case HFCManagerContainer::Products:                                managers_[HFCManagerContainer::Products]                    = new ProductManager(this); break;
			case HFCManagerContainer::ProductDefaults:                         managers_[HFCManagerContainer::ProductDefaults]             = new ProductDefaultManager(this); break;
			case HFCManagerContainer::ProductDetailedStatistics:               break;
			case HFCManagerContainer::ProductStatistics:                       break;
			case HFCManagerContainer::ProductTextCfgs:                         managers_[HFCManagerContainer::ProductTextCfgs]             = new ProductTextCfgManager(this); break;
			case HFCManagerContainer::ProductTotals:                           managers_[HFCManagerContainer::ProductTotals]               = new ProductTotalManager(this); break;
			case HFCManagerContainer::ROMLabels:                               break;
			case HFCManagerContainer::Scales:                                  managers_[HFCManagerContainer::Scales]                      = new ScaleManager(this); break;
			case HFCManagerContainer::SoftKeys:                                managers_[HFCManagerContainer::SoftKeys]                    = new SoftkeyManager(this); break;
			case HFCManagerContainer::SoftKeyFeatures:                         managers_[HFCManagerContainer::SoftKeyFeatures]             = new SoftkeyFeatureManager(this); break;
			case HFCManagerContainer::SoftKeyGroups:                           managers_[HFCManagerContainer::SoftKeyGroups]               = new SoftkeyGroupManager(this); break;
			case HFCManagerContainer::SoftKeyStyles:                           managers_[HFCManagerContainer::SoftKeyStyles]               = new SoftkeyStyleManager(this); break;
			case HFCManagerContainer::SpecialPrograms:                         managers_[HFCManagerContainer::SpecialPrograms]             = new SpecialsManager(this); break;
			case HFCManagerContainer::Status:                                  break;
			case HFCManagerContainer::StreamingGraphics:                       managers_[HFCManagerContainer::StreamingGraphics]           = new StreamingGraphicManager(this); break;
			case HFCManagerContainer::SynchronizationChanges:                  managers_[HFCManagerContainer::SynchronizationChanges]      = new SynchronizationChangeManager(this); break;
			case HFCManagerContainer::Texts:                                   managers_[HFCManagerContainer::Texts]                       = new ProductTextManager(psttExpandedText, this); break;
			case HFCManagerContainer::Transactions:                            managers_[HFCManagerContainer::Transactions]                = new TransactionManager(this); break;
			case HFCManagerContainer::TransactionTypes:                        managers_[HFCManagerContainer::TransactionTypes]            = new TransactionTypeManager(this); break;
			case HFCManagerContainer::WrapperKnifeTime:						   managers_[HFCManagerContainer::WrapperKnifeTime]			   = new EvolWrapperKnifeHistoryManager(this); break;
			case HFCManagerContainer::WrapperErrorHistory:					   managers_[HFCManagerContainer::WrapperErrorHistory]		   = new EvolWrapperErrorHistoryManager(this); break;

			case HFCManagerContainer::COOL:                                    managers_[HFCManagerContainer::COOL]                        = new COOLManager(this); break;
			case HFCManagerContainer::Graphics:                                managers_[HFCManagerContainer::Graphics]                    = new GraphicManager(this); break;
			case HFCManagerContainer::LabelGraphics:                           managers_[HFCManagerContainer::LabelGraphics]               = new LabelGraphicManager(this); break;
			case HFCManagerContainer::CategoryGraphics:                        managers_[HFCManagerContainer::CategoryGraphics]            = new CategoryGraphicManager(this); break;
			case HFCManagerContainer::ProductGraphics:                         managers_[HFCManagerContainer::ProductGraphics]             = new ProductGraphicManager(this); break;
			case HFCManagerContainer::SoftkeyGraphics:                         managers_[HFCManagerContainer::SoftkeyGraphics]             = new SoftkeyGraphicManager(this); break;
			case HFCManagerContainer::SpecialsGraphics:                        managers_[HFCManagerContainer::SpecialsGraphics]            = new SpecialsGraphicManager(this); break;
			case HFCManagerContainer::Peripherals:                             managers_[HFCManagerContainer::Peripherals]                 = new PeripheralManager(this); break;
			case HFCManagerContainer::PlanogramGraphics:                       managers_[HFCManagerContainer::PlanogramGraphics]           = new PlanogramGraphicManager(this); break;

			case HFCManagerContainer::ProductText5:                            managers_[HFCManagerContainer::ProductText5]                = new ProductTextManager(psttProductText5, this); break;
			case HFCManagerContainer::ProductText6:                            managers_[HFCManagerContainer::ProductText6]                = new ProductTextManager(psttProductText6, this); break;
			case HFCManagerContainer::ProductText7:                            managers_[HFCManagerContainer::ProductText7]                = new ProductTextManager(psttProductText7, this); break;
			case HFCManagerContainer::ProductText8:                            managers_[HFCManagerContainer::ProductText8]                = new ProductTextManager(psttProductText8, this); break;
			case HFCManagerContainer::ProductText9:                            managers_[HFCManagerContainer::ProductText9]                = new ProductTextManager(psttProductText9, this); break;

			case HFCManagerContainer::Recalls:                                 managers_[HFCManagerContainer::Recalls]                     = new RecallManager(this); break;

            default:
                // Nothing
				return 0;
                break;
		}

		return managers_[type];
	}


	void HFCManagerContainer::destroyQueriesAndIters()
	{
		Debug::trace("HFCManagerContainer", "destroyQueriesAndIters() - begin");

		for ( int i = 0; i < HFCManagerContainer::LastRecordManager; ++i )
		{
			if ( managers_[i] )
				managers_[i]->destroyQueriesAndIters();
		}

		Debug::trace("HFCManagerContainer", "destroyQueriesAndIters() - end");
	}


	void HFCManagerContainer::initData()
	{
		Debug::trace("HFCManagerContainer", "initData() - entry");

		try
		{
			initDB();

			if (dbMgr)  // defined in DBAccess class
			{
				Debug::trace("HFCManagerContainer", "initData() after DBManager initialized - database is not null");
				/* Go ahead and open the database.  The first user to initialize the database will
				 * open it (which happens to occur in Main).  No one else needs to open it, but if
				 * it did get closed, opening it here will reopen it for everyone.
				 */
				dbMgr->openDatabase();
				Debug::trace("HFCManagerContainer", "initData() after openDatabase() - database is now open");
			}
			else
				Debug::trace("HFCManagerContainer", "initData() after DBManager initialized - database IS NULL!!!");

			Debug::trace("HFCManagerContainer", "initData() - finished");

		}
		catch (DBException &dbe)
		{
			if (Debug::isTracing("DBAccess.exception"))
			{
				Debug::trace("DBAccess.exception", HobString("DBSession::initData() ********** database exception **********"));
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}


	void HFCManagerContainer::uninitData()
	{
		Debug::trace("HFCManagerContainer", "uninitData() - entry");

		try
		{
			destroyQueriesAndIters();
			uninitDB();
		}
		catch (DBException &dbe)
		{
			if (Debug::isTracing("DBAccess.exception"))
			{
				Debug::trace("DBAccess.exception", HobString("DBSession::uninitData() ********** database exception **********"));
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}

		Debug::trace("HFCManagerContainer", "uninitData() - exit");
	}


	void HFCManagerContainer::showDatabaseException(const std::string & module, const edb::DBException & dbe)
	{
		if ( Debug::isTracing("HFCManagerContainer.exception") )
		{
			Debug::trace("HFCManagerContainer.exception", "HFCManagerContainer::" +
				module + " ********** database exception **********");

			Debug::trace("HFCManagerContainer.exception", "   DBException: type: " +
				util_string::inttostring(dbe.getType()) + "  info: " + dbe.info());
		}
	}


	void HFCManagerContainer::initializeTypeContainers()
	{
		/* Initialize Synchronized Manager types. */
		/* Initialize all types to NOT synchronized*/
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::Configurations, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::EmailAlerts, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::ProductDefaults, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::Classes, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::Texts, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::Messages, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::Nutritions, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::Notes, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::Marquees, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::Transactions, false));

		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::TransactionTypes, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::Categories, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::SpecialPrograms, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::CoolCategories, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::CoolCountries, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::CoolProductionSteps, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::CoolPreTexts, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::CoolTexts, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::CoolListTexts, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::CoolLists, false));

		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::CoolCategoryCountries, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::CoolCategoryProductionSteps, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::CoolCategoryPreTexts, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::Products, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::ExpirationLabels, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::Labels, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::LabelTexts, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::OperatorNotes, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::Operators, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::SoftKeyGroups, false));

		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::SoftKeyStyles, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::SoftKeyFeatures, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::SoftKeys, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::LabelGraphics, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::CategoryGraphics, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::ProductGraphics, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::SoftkeyGraphics, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::SpecialsGraphics, false));
		//_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::PlanogramGraphics, false));

		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::ProductText5, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::ProductText6, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::ProductText7, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::ProductText8, false));
		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::ProductText9, false));

		_synchronizedTypes_.insert(std::make_pair(HFCManagerContainer::Recalls, false));


		_deleteAllFilesTypes_.push_back(HFCManagerContainer::Categories);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::Classes);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::CoolCategories);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::CoolCategoryCountries);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::CoolCategoryPreTexts);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::CoolCategoryProductionSteps);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::CoolCountries);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::CoolLists);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::CoolListTexts);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::CoolPreTexts);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::CoolProductionSteps);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::CoolTexts);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::CutTests);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::CutTestTotals);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::ExpirationLabels);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::FlashFiles);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::Labels);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::LabelTexts);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::Marquees);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::Messages);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::Notes);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::Nutritions);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::Operators);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::OperatorNotes);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::Products);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::SoftKeys);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::SoftKeyFeatures);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::SoftKeyGroups);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::SoftKeyStyles);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::SpecialPrograms);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::Texts);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::Transactions);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::TransactionTypes);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::WrapperKnifeTime);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::WrapperErrorHistory);

		_deleteAllFilesTypes_.push_back(HFCManagerContainer::ProductText5);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::ProductText6);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::ProductText7);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::ProductText8);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::ProductText9);

		_deleteAllFilesTypes_.push_back(HFCManagerContainer::LabelGraphics);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::ProductGraphics);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::CategoryGraphics);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::SpecialsGraphics);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::SoftkeyGraphics);
		// not for the HLX   _deleteAllFilesTypes_.push_back(HFCManagerContainer::StreamingGraphicsGraphics);
		_deleteAllFilesTypes_.push_back(HFCManagerContainer::PlanogramGraphics);

		_deleteAllFilesTypes_.push_back(HFCManagerContainer::Recalls);

		_deleteAllFilesTypes_.push_back(HFCManagerContainer::ProductTextCfgs);

		_deleteAllFilesTypes_.push_back(HFCManagerContainer::BarcodeScanners);

		_deleteAllFilesTypes_.push_back(HFCManagerContainer::Depts);


		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::CoolCategoryCountries);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::CoolCategoryPreTexts);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::CoolCategoryProductionSteps);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::CoolCategories);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::CoolCountries);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::CoolPreTexts);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::CoolProductionSteps);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::CoolListTexts);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::CoolLists);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::CoolTexts);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::ExpirationLabels);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::Categories);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::Classes);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::Nutritions);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::Labels);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::Texts);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::Messages);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::Notes);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::Marquees);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::ProductText5);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::ProductText6);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::ProductText7);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::ProductText8);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::ProductText9);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::SoftKeys);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::SoftKeyFeatures);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::SoftKeyGroups);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::SpecialPrograms);
		// _deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::Transactions);  For now, don't delete any transactions when a dept is deleted, which allows the customer to pull them through comm
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::ProductDefaults);
		_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::Products);
		//_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::BarcodeScanners);
		//_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::CutTests);
		//_deleteAllFilesForDeptTypes_.push_back(HFCManagerContainer::CutTestTotals);


		_typeContainersInitialized_ = true;

		/* Those not synchronized:

				ClassTotals,
				HourlyTotals,
				OperatorTotals,
				OperatorChanges,
				ProductTotals,
				CoolTotals,

				Depts,
				Devices,
				FlashFiles,
				NetworkErrors,
				ROMLabels,
				Status,

				Batches,
				Scales,
				SynchronizationChanges,

				CategoryDetailedStatistics,
				CategoryStatistics,
				PresentationMethods,
				PresentationMethodStatistics,
				PresentationMethodDetailedStatistics,
				ProductDetailedStatistics,
				ProductStatistics,
				StreamingGraphics,
		*/
	}


	void HFCManagerContainer::updateSyncList()
	{
		Configuration *pConfig = Configuration::getInstance();
		_synchronizedTypes_[HFCManagerContainer::Configurations] = pConfig->toBoolCfg(fieldOffsetCfgSyncConfigurations);
		_synchronizedTypes_[HFCManagerContainer::EmailAlerts] = pConfig->toBoolCfg(fieldOffsetCfgSyncEmailAlerts);
		_synchronizedTypes_[HFCManagerContainer::ProductDefaults] = pConfig->toBoolCfg(fieldOffsetCfgSyncProductDefaults);
		_synchronizedTypes_[HFCManagerContainer::Classes] = pConfig->toBoolCfg(fieldOffsetCfgSyncClasses);
		_synchronizedTypes_[HFCManagerContainer::Texts] = pConfig->toBoolCfg(fieldOffsetCfgSyncExpandedTexts);
		_synchronizedTypes_[HFCManagerContainer::Messages] = pConfig->toBoolCfg(fieldOffsetCfgSyncSpecialMessages);
		_synchronizedTypes_[HFCManagerContainer::Nutritions] = pConfig->toBoolCfg(fieldOffsetCfgSyncNutritionFacts);
		_synchronizedTypes_[HFCManagerContainer::Notes] = pConfig->toBoolCfg(fieldOffsetCfgSyncProductNotes);
		_synchronizedTypes_[HFCManagerContainer::Marquees] = pConfig->toBoolCfg(fieldOffsetCfgSyncMarqueeTexts);
		_synchronizedTypes_[HFCManagerContainer::Transactions] = pConfig->toBoolCfg(fieldOffsetCfgSyncTransactions);
		
		_synchronizedTypes_[HFCManagerContainer::TransactionTypes] = pConfig->toBoolCfg(fieldOffsetCfgSyncTransactionTypes);
		_synchronizedTypes_[HFCManagerContainer::Categories] = pConfig->toBoolCfg(fieldOffsetCfgSyncCategories);
		_synchronizedTypes_[HFCManagerContainer::SpecialPrograms] = pConfig->toBoolCfg(fieldOffsetCfgSyncSpecials);
		_synchronizedTypes_[HFCManagerContainer::CoolCategories] = pConfig->toBoolCfg(fieldOffsetCfgSyncCoolCategories);
		_synchronizedTypes_[HFCManagerContainer::CoolCountries] = pConfig->toBoolCfg(fieldOffsetCfgSyncCoolCountries);
		_synchronizedTypes_[HFCManagerContainer::CoolProductionSteps] = pConfig->toBoolCfg(fieldOffsetCfgSyncCoolProductionSteps);
		_synchronizedTypes_[HFCManagerContainer::CoolPreTexts] = pConfig->toBoolCfg(fieldOffsetCfgSyncCoolPreTexts);
		_synchronizedTypes_[HFCManagerContainer::CoolTexts] = pConfig->toBoolCfg(fieldOffsetCfgSyncCoolTexts);
		_synchronizedTypes_[HFCManagerContainer::CoolListTexts] = pConfig->toBoolCfg(fieldOffsetCfgSyncCoolListTexts);
		_synchronizedTypes_[HFCManagerContainer::CoolLists] = pConfig->toBoolCfg(fieldOffsetCfgSyncCoolLists);

		_synchronizedTypes_[HFCManagerContainer::CoolCategoryCountries] = pConfig->toBoolCfg(fieldOffsetCfgSyncCategoryCountries);
		_synchronizedTypes_[HFCManagerContainer::CoolCategoryProductionSteps] = pConfig->toBoolCfg(fieldOffsetCfgSyncCategoryProductionSteps);
		_synchronizedTypes_[HFCManagerContainer::CoolCategoryPreTexts] = pConfig->toBoolCfg(fieldOffsetCfgSyncCategoryPreTexts);
		_synchronizedTypes_[HFCManagerContainer::Products] = pConfig->toBoolCfg(fieldOffsetCfgSyncProducts);
		_synchronizedTypes_[HFCManagerContainer::ExpirationLabels] = pConfig->toBoolCfg(fieldOffsetCfgSyncExpirationLabels);
		_synchronizedTypes_[HFCManagerContainer::Labels] = pConfig->toBoolCfg(fieldOffsetCfgSyncLabels);
		_synchronizedTypes_[HFCManagerContainer::LabelTexts] = pConfig->toBoolCfg(fieldOffsetCfgSyncLabelTexts);
		_synchronizedTypes_[HFCManagerContainer::OperatorNotes] = pConfig->toBoolCfg(fieldOffsetCfgSyncOperatorNotes);
		_synchronizedTypes_[HFCManagerContainer::Operators] = pConfig->toBoolCfg(fieldOffsetCfgSyncOperators);
		_synchronizedTypes_[HFCManagerContainer::SoftKeyGroups] = pConfig->toBoolCfg(fieldOffsetCfgSyncSoftkeyGroups);

		_synchronizedTypes_[HFCManagerContainer::SoftKeyStyles] = pConfig->toBoolCfg(fieldOffsetCfgSyncSoftkeyStyles);
		_synchronizedTypes_[HFCManagerContainer::SoftKeyFeatures] = pConfig->toBoolCfg(fieldOffsetCfgSyncSoftkeyFeatures);
		_synchronizedTypes_[HFCManagerContainer::SoftKeys] = pConfig->toBoolCfg(fieldOffsetCfgSyncSoftkeys);
		_synchronizedTypes_[HFCManagerContainer::LabelGraphics] = pConfig->toBoolCfg(fieldOffsetCfgSyncLabelGraphics);
		_synchronizedTypes_[HFCManagerContainer::CategoryGraphics] = pConfig->toBoolCfg(fieldOffsetCfgSyncCategoryGraphics);
		_synchronizedTypes_[HFCManagerContainer::ProductGraphics] = pConfig->toBoolCfg(fieldOffsetCfgSyncProductGraphics);
		_synchronizedTypes_[HFCManagerContainer::SoftkeyGraphics] = pConfig->toBoolCfg(fieldOffsetCfgSyncSoftkeyGraphics);
		_synchronizedTypes_[HFCManagerContainer::SpecialsGraphics] = pConfig->toBoolCfg(fieldOffsetCfgSyncSpecialsGraphics);
		//_synchronizedTypes_[HFCManagerContainer::PlanogramGraphics] = pConfig->toBoolCfg();

		_synchronizedTypes_[HFCManagerContainer::ProductText5] = pConfig->toBoolCfg(fieldOffsetCfgSyncExpandedTexts);  // leave as using the exp text flag for now
		_synchronizedTypes_[HFCManagerContainer::ProductText6] = pConfig->toBoolCfg(fieldOffsetCfgSyncExpandedTexts);
		_synchronizedTypes_[HFCManagerContainer::ProductText7] = pConfig->toBoolCfg(fieldOffsetCfgSyncExpandedTexts);
		_synchronizedTypes_[HFCManagerContainer::ProductText8] = pConfig->toBoolCfg(fieldOffsetCfgSyncExpandedTexts);
		_synchronizedTypes_[HFCManagerContainer::ProductText9] = pConfig->toBoolCfg(fieldOffsetCfgSyncExpandedTexts);

		//_synchronizedTypes_[HFCManagerContainer::Recalls] = pConfig->getSyncRecalls();
	}


	bool HFCManagerContainer::isSynchronized(HFCManagerContainer::HFCRecordManagerType type)
	{
		syncListIterator_type iter = _synchronizedTypes_.find(type);

		if (iter != _synchronizedTypes_.end())
			return iter->second;

		return false;
	}

	HFCManagerContainer::syncListIterator_type HFCManagerContainer::beginSynchronizedDataManager() 
	{
		syncListIterator_type iter = _synchronizedTypes_.begin();

		while ((iter != _synchronizedTypes_.end()) && !iter->second)
			++iter;

		return iter; 
	}

	HFCManagerContainer::syncListIterator_type HFCManagerContainer::nextSynchronizedDataManager(HFCManagerContainer::syncListIterator_type iter) 
	{
		if (iter == _synchronizedTypes_.end())
			return iter;

		do
		{
			++iter;
		}
		while ((iter != _synchronizedTypes_.end()) && !iter->second);

		return iter; 
	}


}
