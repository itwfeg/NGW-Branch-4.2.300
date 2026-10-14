/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLMANAGER_H
#define COOLMANAGER_H

#include "evolution.h"
#include "DataManager.h"

#include "DBCoolCategoriesRM.h"
#include "DBCoolCountriesRM.h"
#include "DBCoolProductionStepsRM.h"
#include "DBCoolPreTextsRM.h"
#include "DBCoolCategoryCountriesRM.h"
#include "DBCoolCategoryProductionStepsRM.h"
#include "DBCoolCategoryPreTextsRM.h"
#include "DBCoolTextsRM.h"
#include "DBCoolListsRM.h"
#include "DBCoolListTextsRM.h"
#include "DBProductsRM.h"

#include "DBQuery.h"
#include "DBCoolCategory.h"
#include "DBCoolCountry.h"
#include "DBCoolProductionStep.h"
#include "DBCoolPreText.h"
#include "DBCoolText.h"
#include "DBCoolList.h"
#include "DBCoolListText.h"
#include "DBProduct.h"

#include "DBCoolCategoryCountry.h"
#include "DBCoolCategoryPreText.h"
#include "DBCoolCategoryProductionStep.h"

class DBSession;
class AllCoolCategories;
class AllCoolCountries;
class AllCoolProductionSteps;
class AllCoolPretexts;
class AllCoolTexts;
class COOLCategory;
class COOLCountry;
class COOLProductionStep;
class COOLPretext;
class COOLText;

class COOLShortlist;
class COOLList;
class COOLListText;

class COOLCategoryCountry;
class COOLCategoryPretext;
class COOLCategoryProdStep;

namespace hfc
{
	class HFCManagerContainer;
}


class COOLManager : public DataManager
{
	public:

		COOLManager();
		COOLManager(hfc::HFCManagerContainer *container);
		virtual ~COOLManager();

		void handleMessage(SessionMessage *msg);

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		//COOL* getFirstRecord() { return firstCOOL(); }
		//COOL* getNextRecord()  { return nextCOOL(); }
		EvolDeviceType getDevice() const { return COOLDev; }

		COOLCategory *firstCOOLCategory();
		COOLCategory *nextCOOLCategory();
		int countAllCOOLCategories();

		COOLCountry *firstCOOLCountry();
		COOLCountry *nextCOOLCountry();
		int countAllCOOLCountries();
		int countAllCOOLCountriesForAllCategories();

		COOLProductionStep *firstCOOLProductionStep();
		COOLProductionStep *nextCOOLProductionStep();
		int countAllCOOLProductionSteps();
		int countAllCOOLProductionStepsForAllCategories();

		COOLPretext *firstCOOLPretext();
		COOLPretext *nextCOOLPretext();
		int countAllCOOLPretexts();

		COOLText *firstCOOLText();
		COOLText *nextCOOLText();
		int countAllCOOLTexts();

		COOLShortlist *firstCOOLShortlist();
		COOLShortlist *nextCOOLShortlist();
		int countAllCOOLShortlists();

		COOLList *firstCOOLList();
		COOLList *nextCOOLList();
		int countAllCOOLLists();

		COOLListText *firstCOOLListText();
		COOLListText *nextCOOLListText();
		int countAllCOOLListTexts();

		COOLCategoryCountry *firstCOOLCategoryCountry();
		COOLCategoryCountry *nextCOOLCategoryCountry();
		int countAllCOOLCategoryCountries();

		COOLCategoryPretext *firstCOOLCategoryPretext();
		COOLCategoryPretext *nextCOOLCategoryPretext();
		int countAllCOOLCategoryPretexts();

		COOLCategoryProdStep *firstCOOLCategoryProductionStep();
		COOLCategoryProdStep *nextCOOLCategoryProductionStep();
		int countAllCOOLCategoryProductionSteps();


		void handleDeleteAllCOOLCategories(HobRecordNum deptNum = RECORD_NOT_FOUND);
		void handleDeleteAllCOOLCountries(HobRecordNum deptNum = RECORD_NOT_FOUND);
		void handleDeleteAllCOOLProductionSteps(HobRecordNum deptNum = RECORD_NOT_FOUND);
		void handleDeleteAllCOOLPretexts(HobRecordNum deptNum = RECORD_NOT_FOUND);
		void handleDeleteAllCOOLTexts(HobRecordNum deptNum = RECORD_NOT_FOUND);
		void handleDeleteAllCOOLShortlists(HobRecordNum deptNum = RECORD_NOT_FOUND);
		void handleDeleteAllCOOLLists(HobRecordNum deptNum = RECORD_NOT_FOUND);
		void handleDeleteAllCOOLListTexts(HobRecordNum deptNum = RECORD_NOT_FOUND);


		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		COOLCategory		*currentCategory;
		COOLCountry			*currentCountry;
		COOLProductionStep	*currentProductionStep;
		COOLPretext			*currentPretext;
		COOLText			*currentCoolText;
		COOLShortlist		*currentCoolShortlist;

		edb::DBQuery<edb::DBCoolCategory> *allCoolCategoriesQuery;
		edb::DBQuery<edb::DBCoolCategory>::const_iterator *allCoolCategoriesIter;

		edb::DBQuery<edb::DBCoolCountry> *allCoolCountriesQuery;
		edb::DBQuery<edb::DBCoolCountry>::const_iterator *allCoolCountriesIter;

		edb::DBQuery<edb::DBCoolProductionStep> *allCoolProductionStepsQuery;
		edb::DBQuery<edb::DBCoolProductionStep>::const_iterator *allCoolProductionStepsIter;

		edb::DBQuery<edb::DBCoolPreText> *allCoolPretextsQuery;
		edb::DBQuery<edb::DBCoolPreText>::const_iterator *allCoolPretextsIter;

		edb::DBQuery<edb::DBCoolText> *allCoolTextsQuery;
		edb::DBQuery<edb::DBCoolText>::const_iterator *allCoolTextsIter;

		edb::DBQuery<edb::DBCoolList> *allCoolListsQuery;
		edb::DBQuery<edb::DBCoolList>::const_iterator *allCoolListsIter;

		edb::DBQuery<edb::DBCoolListText> *allCoolListTextsQuery;
		edb::DBQuery<edb::DBCoolListText>::const_iterator *allCoolListTextsIter;

		edb::DBQuery<edb::DBCoolCategoryCountry> *allCoolCategoryCountriesQuery;
		edb::DBQuery<edb::DBCoolCategoryCountry>::const_iterator *allCoolCategoryCountriesIter;

		edb::DBQuery<edb::DBCoolCategoryPreText> *allCoolCategoryPretextsQuery;
		edb::DBQuery<edb::DBCoolCategoryPreText>::const_iterator *allCoolCategoryPretextsIter;

		edb::DBQuery<edb::DBCoolCategoryProductionStep> *allCoolCategoryProductionStepsQuery;
		edb::DBQuery<edb::DBCoolCategoryProductionStep>::const_iterator *allCoolCategoryProductionStepsIter;

		edb::DBCoolList *theDbCoolList;

		void handleCOOLCategoryMessage(SessionMessage *msg);
		COOLCategory *getCurrentCategory();
		void setCurrentCategory(COOLCategory *category);
		void handleReadCOOLCategory(COOLCategory *msgCategory);
		void handleReadAllCOOLCategories();
		void handleWriteCOOLCategory(COOLCategory *msgCategory);
		void handleDeleteCOOLCategory(SessionMessage *msg);
		void handleSaveCOOLCategory(COOLCategory *msgCategory);
		void handleRestoreCOOLCategory(SessionMessage *msg);
		void handleCreateCOOLCategory(COOLCategory *msgCategory);
		void handleReadAllCOOLCategoriesByCountry(AllCoolCategories *msgCategory);
		void handleReadAllCOOLCategoriesByProdStep(AllCoolCategories *msgCategory);
		void handleReadAllCOOLCategoriesByPretext(AllCoolCategories *msgCategory);

		void handleCOOLCountryMessage(SessionMessage *msg);
		COOLCountry *getCurrentCountry();
		void setCurrentCountry(COOLCountry *country);
		void handleReadCOOLCountry(COOLCountry *msgCountry);
		void handleReadAllCOOLCountries(AllCoolCountries *msgAll);
		void handleWriteCOOLCountry(COOLCountry *msgCountry);
		void handleDeleteCOOLCountry(SessionMessage *msg);
		void handleUpdateAndSaveCOOLCountry(COOLCountry *msgCountry);
		void handleSaveRecordCOOLCountry(COOLCountry *recordToSave);
		void handleRestoreCOOLCountry(SessionMessage *msg);
		void handleCreateCOOLCountry();
		void handleReadCOOLCountriesByOneCategory(AllCoolCountries *msgAll);
		void handleAddCOOLCountryToCategory(COOLCountry *msgCountry);
		void handleDeleteCOOLCountryFromCategory(COOLCountry *msgCountry);
		void handleCreateCOOLCountryAndSave(COOLCountry *msgCountry);
		void handleReadCOOLCountriesByOneCategoryAndAllCategories(AllCoolCountries *msgAll);
		void handleReadCOOLCountriesByNotAllCategories(AllCoolCountries *msgAll);

		void handleCoolProductionStepMessage(SessionMessage *msg);
		COOLProductionStep *getCurrentProductionStep();
		void setCurrentProductionStep(COOLProductionStep *country);
		void handleReadCOOLProductionStep(COOLProductionStep *msgProductionStep);
		void handleReadAllCOOLProductionSteps(AllCoolProductionSteps *myList);
		void handleWriteCOOLProductionStep(COOLProductionStep *msgProductionStep);
		void handleDeleteCOOLProductionStep(SessionMessage *msg);
		void handleUpdateAndSaveCOOLProductionStep(COOLProductionStep *msgProductionStep);
		void handleSaveRecordCOOLProductionStep(COOLProductionStep *recordToSave);
		void handleRestoreCOOLProductionStep(SessionMessage *msg);
		void handleCreateCOOLProductionStep();
		void handleReadCOOLProductionStepsByOneCategory(AllCoolProductionSteps *myList);
		void handleAddCOOLProductionStepToCategory(COOLProductionStep *msgProductionStep);
		void handleDeleteCOOLProductionStepFromCategory(COOLProductionStep *msgProductionStep);
		void handleCreateCOOLProductionStepAndSave(COOLProductionStep *msgProductionStep);
		void handleReadCOOLProdStepsByOneCategoryAndAllCategories(AllCoolProductionSteps *myList);
		void handleReadCOOLProdStepsByNotAllCategories(AllCoolProductionSteps *myList);

		void handleCOOLPretextMessage(SessionMessage *msg);
		COOLPretext *getCurrentPretext();
		void setCurrentPretext(COOLPretext *country);
		void handleReadCOOLPretext(COOLPretext *msgPretext);
		void handleReadAllCOOLPretexts(AllCoolPretexts *myList);
		void handleWriteCOOLPretext(COOLPretext *msgPretext);
		void handleDeleteCOOLPretext(SessionMessage *msg);
		void handleUpdateAndSaveCOOLPretext(COOLPretext *msgPretext);
		void handleSaveRecordCOOLPretext(COOLPretext *recordToSave);
		void handleRestoreCOOLPretext(SessionMessage *msg);
		void handleCreateCOOLPretext(COOLPretext *msgPretext);
		void handleReadCOOLPretextsByOneCategory(AllCoolPretexts *myList);
		void handleAddCOOLPretextToCategory(COOLPretext *msgPretext);
		void handleDeleteCOOLPretextFromCategory(COOLPretext *msgPretext);
		void handleReadCOOLPretextsByOneCategoryAndAllCategories(AllCoolPretexts *myList);
		void handleReadCOOLPretextsByNotAllCategories(AllCoolPretexts *myList);

		void handleCOOLTextMessage(SessionMessage *msg);
		COOLText *getCurrentCoolText();
		void setCurrentCoolText(COOLText *text);
		void handleReadCOOLText(COOLText *msgCoolText);
		void handleReadAllCOOLTexts();
		void handleWriteCOOLText(COOLText *msgCoolText);
		void handleDeleteCOOLText(SessionMessage *msg);
		void handleSaveCOOLText(COOLText *msgCoolText);
		void handleRestoreCOOLText(SessionMessage *msg);
		void handleCreateCOOLText();

		void handleCOOLShortlistMessage(SessionMessage *msg);
		void handleCOOLShortlistReadAll(AllCoolTexts *myList);
		void handleCOOLShortlistAddText(SessionMessage *msg);
		void handleCOOLShortlistDeleteText(SessionMessage *msg);

		HobRecordNum findNewCoolTextNumber();

		void handleWriteCOOLShortlist(COOLShortlist *restoreShortlist);
		void handleRestoreHobDiskShortlist(SessionMessage *msg);
		void handleRestoreCOOLShortlistText(COOLListText *restoreCoolListText);
		void handleRestoreCOOLShortlist(SessionMessage * msg);
		void handleRestoreCOOLShortlistForSync(SessionMessage * msg);

		edb::DBCoolList *createNewCoolShortlistRecord(HobRecordNum startingPointForRecordNum, HobRecordNum deptNum);
		void appendToCoolListTextRec(edb::DBCoolListTextsRM *dbCoolListTextsRM, edb::DBCoolListText *shortlistTextRec, edb::DBCoolList *dbCoolListRec, COOLShortlist *shortlist, int order);

		void handleCOOLCategoryCountryMessage(SessionMessage *msg);
		void handleDeleteCOOLCategoryCountry(SessionMessage *msg);
		void handleDeleteAllCOOLCategoryCountries(HobRecordNum deptNum = RECORD_NOT_FOUND);
		void handleSaveRecordCOOLCategoryCountry(COOLCategoryCountry *recordToSave);
		void handleRestoreCOOLCategoryCountry(SessionMessage *msg);

		void handleCOOLCategoryPretextMessage(SessionMessage *msg);
		void handleDeleteCOOLCategoryPretext(SessionMessage *msg);
		void handleDeleteAllCOOLCategoryPretexts(HobRecordNum deptNum = RECORD_NOT_FOUND);
		void handleSaveRecordCOOLCategoryPretext(COOLCategoryPretext *recordToSave);
		void handleRestoreCOOLCategoryPretext(SessionMessage *msg);

		void handleCOOLCategoryProductionStepMessage(SessionMessage *msg);
		void handleDeleteCOOLCategoryProductionStep(SessionMessage *msg);
		void handleDeleteAllCOOLCategoryProductionSteps(HobRecordNum deptNum = RECORD_NOT_FOUND);
		void handleSaveRecordCOOLCategoryProductionStep(COOLCategoryProdStep *recordToSave);
		void handleRestoreCOOLCategoryProductionStep(SessionMessage *msg);


		void destroyCOOLCategoriesQueryAndIter();
		void destroyCOOLCountriesQueryAndIter();
		void destroyCOOLProductionStepsQueryAndIter();
		void destroyCOOLPretextsQueryAndIter();
		void destroyCOOLTextsQueryAndIter();
		void destroyCOOLShortlistsQueryAndIter();
		void destroyCOOLListTextsQueryAndIter();
		void destroyCOOLListsQueryAndIter();
		void destroyCOOLCategoryCountriesQueryAndIter();
		void destroyCOOLCategoryPretextsQueryAndIter();
		void destroyCOOLCategoryProductionStepsQueryAndIter();

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //COOLMANAGER_H
