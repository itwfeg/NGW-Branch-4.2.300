/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */
#include "COOLManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "DBSession.h"
#include "ResponseMessageManager.h"
#include "ProductManager.h"
#include "Product.h"
#include "HfcError.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

//#include "DBAccess.h"

#include "COOLDevEvent.h"

#include "AllCoolCategories.h"
#include "AllCoolCountries.h"
#include "AllCoolProductionSteps.h"
#include "AllCoolPretexts.h"
#include "COOLCategory.h"
#include "COOLCountry.h"
#include "COOLProductionStep.h"
#include "COOLPretext.h"
#include "COOLText.h"
#include "COOLShortlist.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int COOLManager::dataType = 0;
std::string COOLManager::dataTypeName("COOLManager"); 
#endif


COOLManager::COOLManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

COOLManager::COOLManager(hfc::HFCManagerContainer *container)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	dbAccess = container;
	mgrContainer_ = container;
}

COOLManager::~COOLManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	destroyQueriesAndIters();
}


void COOLManager::init()
{
	mySession = 0;

	currentCategory			= 0;
	currentCountry			= 0;
	currentProductionStep	= 0;
	currentPretext			= 0;
	currentCoolText			= 0;
	currentCoolShortlist	= 0;

	allCoolCategoriesQuery = 0;
	allCoolCategoriesIter  = 0;

	allCoolCountriesQuery = 0;
	allCoolCountriesIter = 0;

	allCoolProductionStepsQuery = 0;
	allCoolProductionStepsIter = 0;

	allCoolPretextsQuery = 0;
	allCoolPretextsIter = 0;

	allCoolTextsQuery = 0;
	allCoolTextsIter = 0;

	allCoolListTextsQuery = 0;
	allCoolListTextsIter = 0;

	allCoolListsQuery = 0;
	allCoolListsIter = 0;

	allCoolCategoryCountriesQuery = 0;
	allCoolCategoryCountriesIter = 0;

	allCoolCategoryPretextsQuery = 0;
	allCoolCategoryPretextsIter = 0;

	allCoolCategoryProductionStepsQuery = 0;
	allCoolCategoryProductionStepsIter = 0;

	theDbCoolList = 0;
}


DBSession *COOLManager::getMySession()
{
	return mySession;
}
void COOLManager::setMySession(DBSession *session)
{
	mySession = session;
}


void COOLManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	if (Debug::isTracing("COOLManager"))
	{
		HobString myDebug("handling event: ");
		myDebug.append(Integer::toString(event));
		Debug::trace("COOLManager", myDebug);
	}
	switch (msg->getEventType())
	{
/*      **********  Categories  **********      */

		case COOLDevReadCategory:
		case COOLDevReadAllCategories:
		case COOLDevWriteCategory:
		case COOLDevDeleteCategory:
		case COOLDevDeleteAllCategories:
		case COOLDevSaveCategory:
		case COOLDevRestoreCategory:
		case COOLDevCreateNewCategory:
		case COOLDevReadAllCategoriesByCountry:
		case COOLDevReadAllCategoriesByProdStep:
		case COOLDevReadAllCategoriesByPretext:
		case COOLDevCategoryBatchApplyDelete:
			handleCOOLCategoryMessage(msg);
			break;

/*      **********  Countries  **********       */

		case COOLDevReadCountry:
		case COOLDevReadAllCountries:
		case COOLDevWriteCountry:
		case COOLDevDeleteCountry:
		case COOLDevDeleteAllCountries:
		case COOLDevSaveCountry:
		case COOLDevRestoreCountry:
		case COOLDevCreateNewCountry:
		case COOLDevReadByCategoryOnlyCountries:
		case COOLDevAddCountryToCategory:
		case COOLDevDeleteCountryFromCategory:
		case COOLDevCreateNewCountryAndSave:
		case COOLDevReadByAllAndByCategoryCountries:
		case COOLDevCountryBatchApplyDelete:
			handleCOOLCountryMessage(msg);
			break;

/*      **********  ProductionSteps  **********     */

		case COOLDevReadProductionStep:
		case COOLDevReadAllProductionSteps:
		case COOLDevWriteProductionStep:
		case COOLDevDeleteProductionStep:
		case COOLDevDeleteAllProductionSteps:
		case COOLDevSaveProductionStep:
		case COOLDevRestoreProductionStep:
		case COOLDevCreateNewProductionStep:
		case COOLDevReadByCategoryOnlyProductionSteps:
		case COOLDevAddProductionStepToCategory:
		case COOLDevDeleteProductionStepFromCategory:
		case COOLDevCreateNewProductionStepAndSave:
		case COOLDevReadByAllAndByCategoryProductionSteps:
		case COOLDevProductionStepBatchApplyDelete:
			handleCoolProductionStepMessage(msg);
			break;

/*      **********  Pretexts  **********        */

		case COOLDevReadPretext:
		case COOLDevReadAllPretexts:
		case COOLDevWritePretext:
		case COOLDevDeletePretext:
		case COOLDevDeleteAllPretexts:
		case COOLDevSavePretext:
		case COOLDevRestorePretext:
		case COOLDevCreateNewPretext:
		case COOLDevReadByCategoryOnlyPretexts:
		case COOLDevAddPretextToCategory:
		case COOLDevDeletePretextFromCategory:
		case COOLDevReadByAllAndByCategoryPretexts:
		case COOLDevPretextBatchApplyDelete:
			handleCOOLPretextMessage(msg);
			break;

/*      **********  COOL Text  **********       */
		case COOLDevReadCoolText:
		case COOLDevReadAllCoolText:
		case COOLDevWriteCoolText:
		case COOLDevDeleteCoolText:
		case COOLDevDeleteAllCoolText:
		case COOLDevSaveCoolText:
		case COOLDevRestoreCoolText:
		case COOLDevCreateNewCoolText:
		case COOLDevReadAllCoolTextForUltima:
		case COOLDevWriteAllCoolTextForUltima:
		case COOLDevCoolTextBatchApplyDelete:
			handleCOOLTextMessage(msg);
			break;

		case COOLDevDeleteAllCountryTextForUltima:		//	Eventually change to COOLDevDeleteAllCoolTextForUltima
		{
			handleCOOLTextMessage(msg);			//	This will delete all cool texts
			handleCOOLPretextMessage(msg);		//	This will delete all pretexts
			break;
		}

/*      **********  COOL Shortlist  **********       */

		case COOLDevShortlistReadAllCoolText:
		case COOLDevShortlistAddText:
		case COOLDevShortlistDeleteText:

		case COOLDevRestoreShortlistText:
		case COOLDevRestoreShortlist:
		case COOLDevRestoreShortlistForUltima:

		case COOLDevReadAllShortlistsForUltima:
		case COOLDevWriteAllShortlistsForUltima:

		case COOLDevDeleteShortlist:
		case COOLDevDeleteAllShortlists:
		case COOLDevDeleteAllShortlistTexts:
			handleCOOLShortlistMessage(msg);
			break;

/*      **********  Category/Countries  **********       */
		case COOLDevRestoreCategoryCountry:
		case COOLDevDeleteCategoryCountry:
		case COOLDevDeleteAllCategoryCountries:
		case COOLDevBatchApplyDeleteCategoryCountry:
		case COOLDevReadAllCategoryCountriesForUltima:
		case COOLDevWriteAllCategoryCountriesForUltima:
			handleCOOLCategoryCountryMessage(msg);
			break;

/*      **********  Category/ProductionSteps  **********      */
		case COOLDevRestoreCategoryProductionStep:
		case COOLDevDeleteCategoryProductionStep:
		case COOLDevDeleteAllCategoryProductionSteps:
		case COOLDevBatchApplyDeleteCategoryProductionStep:
		case COOLDevReadAllCategoryProdStepsForUltima:
		case COOLDevWriteAllCategoryProdStepsForUltima:
			handleCOOLCategoryProductionStepMessage(msg);
			break;

/*      **********  Category/Pretexts  **********      */
		case COOLDevRestoreCategoryPretext:
		case COOLDevDeleteCategoryPretext:
		case COOLDevDeleteAllCategoryPretexts:
		case COOLDevBatchApplyDeleteCategoryPretext:
			handleCOOLCategoryPretextMessage(msg);
			break;

		default:
			if (Debug::isTracing("COOLManager"))
			{
				HobString myDebug("UNKNOWN EVENT: ");
				myDebug.append(Integer::toString(event)).append(" !!!");
				Debug::trace("COOLManager", myDebug);
			}
			break;
	}
}


void COOLManager::destroyQueriesAndIters()
{
	destroyCOOLCategoriesQueryAndIter();
	destroyCOOLCountriesQueryAndIter();
	destroyCOOLProductionStepsQueryAndIter();
	destroyCOOLPretextsQueryAndIter();
	destroyCOOLTextsQueryAndIter();
	destroyCOOLListTextsQueryAndIter();
	destroyCOOLListsQueryAndIter();
	destroyCOOLCategoryCountriesQueryAndIter();
	destroyCOOLCategoryPretextsQueryAndIter();
	destroyCOOLCategoryProductionStepsQueryAndIter();
}

void COOLManager::destroyCOOLCategoriesQueryAndIter()
{
	if (allCoolCategoriesIter)
	{
		delete allCoolCategoriesIter;
		allCoolCategoriesIter = 0;
	}

	if (allCoolCategoriesQuery)
	{
		delete allCoolCategoriesQuery;
		allCoolCategoriesQuery = 0;
	}
}

void COOLManager::destroyCOOLCountriesQueryAndIter()
{
	if (allCoolCountriesIter)
	{
		delete allCoolCountriesIter;
		allCoolCountriesIter = 0;
	}

	if (allCoolCountriesQuery)
	{
		delete allCoolCountriesQuery;
		allCoolCountriesQuery = 0;
	}
}

void COOLManager::destroyCOOLProductionStepsQueryAndIter()
{
	if (allCoolProductionStepsIter)
	{
		delete allCoolProductionStepsIter;
		allCoolProductionStepsIter = 0;
	}

	if (allCoolProductionStepsQuery)
	{
		delete allCoolProductionStepsQuery;
		allCoolProductionStepsQuery = 0;
	}
}

void COOLManager::destroyCOOLPretextsQueryAndIter()
{
	if (allCoolPretextsIter)
	{
		delete allCoolPretextsIter;
		allCoolPretextsIter = 0;
	}

	if (allCoolPretextsQuery)
	{
		delete allCoolPretextsQuery;
		allCoolPretextsQuery = 0;
	}
}

void COOLManager::destroyCOOLTextsQueryAndIter()
{
	if (allCoolTextsIter)
	{
		delete allCoolTextsIter;
		allCoolTextsIter = 0;
	}

	if (allCoolTextsQuery)
	{
		delete allCoolTextsQuery;
		allCoolTextsQuery = 0;
	}
}

void COOLManager::destroyCOOLShortlistsQueryAndIter()
{
	destroyCOOLListTextsQueryAndIter();
}

void COOLManager::destroyCOOLListTextsQueryAndIter()
{
	if (allCoolListTextsIter)
	{
		delete allCoolListTextsIter;
		allCoolListTextsIter = 0;
	}

	if (allCoolListTextsQuery)
	{
		delete allCoolListTextsQuery;
		allCoolListTextsQuery = 0;
	}
}

void COOLManager::destroyCOOLListsQueryAndIter()
{
	if (allCoolListsIter)
	{
		delete allCoolListsIter;
		allCoolListsIter = 0;
	}

	if (allCoolListsQuery)
	{
		delete allCoolListsQuery;
		allCoolListsQuery = 0;
	}
}

void COOLManager::destroyCOOLCategoryCountriesQueryAndIter()
{
	if (allCoolCategoryCountriesIter)
	{
		delete allCoolCategoryCountriesIter;
		allCoolCategoryCountriesIter = 0;
	}

	if (allCoolCategoryCountriesQuery)
	{
		delete allCoolCategoryCountriesQuery;
		allCoolCategoryCountriesQuery = 0;
	}
}

void COOLManager::destroyCOOLCategoryPretextsQueryAndIter()
{
	if (allCoolCategoryPretextsIter)
	{
		delete allCoolCategoryPretextsIter;
		allCoolCategoryPretextsIter = 0;
	}

	if (allCoolCategoryPretextsQuery)
	{
		delete allCoolCategoryPretextsQuery;
		allCoolCategoryPretextsQuery = 0;
	}
}

void COOLManager::destroyCOOLCategoryProductionStepsQueryAndIter()
{
	if (allCoolCategoryProductionStepsIter)
	{
		delete allCoolCategoryProductionStepsIter;
		allCoolCategoryProductionStepsIter = 0;
	}

	if (allCoolCategoryProductionStepsQuery)
	{
		delete allCoolCategoryProductionStepsQuery;
		allCoolCategoryProductionStepsQuery = 0;
	}
}
