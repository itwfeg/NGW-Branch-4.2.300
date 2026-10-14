/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CATEGORYMANAGER_H
#define CATEGORYMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "CategoryEvent.h"
#include "Category.h"
#include "FeaturedCategory.h"

class SessionMessage;
class DBSession;
class SoftKeyLists;

#include "DBCategoriesRM.h"
#include "DBCategory.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}


class CategoryManager : public DataManager
{
	public:

		CategoryManager();
		CategoryManager(hfc::HFCManagerContainer *container);
		virtual ~CategoryManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		Category* getFirstRecord() { return firstCategory(); }
		Category* getNextRecord()  { return nextCategory(); }
		EvolDeviceType getDevice() const { return CategoryDev; }

		Category *firstCategory();
		Category *nextCategory();
		int countAllCategories();

		Category *getCurrentCategory();
		void setCurrentCategory(Category *newCategory);

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();
		void handleDeleteAll(HobRecordNum deptNum);
		void handleSaveRecord(Category *recordToSave);

		Category *createNewCategory(HobRecordNum recordNum);
		void sendCurrentCategory();

		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		Category *currentCategory;

		edb::DBQuery<edb::DBCategory> *allCategoriesQuery;
		edb::DBQuery<edb::DBCategory>::const_iterator *allCategoriesIter;

		SoftKeyLists *currentGridData;


		void init();

		void handleRead(Category *msgCategory);
		void handleReadAll();
		void handleWrite(Category *modifiedCategory);
		void handleDelete(SessionMessage *msg);
		void handleUpdateAndSave(Category *modifiedCategory = 0);
		void handleRestore(SessionMessage *msg);
		void handleCreate(Category *msgCategory);

//		void handleReadSoftKeyList(SessionMessage *msg);
		void handleReadSubCategoriesList(SessionMessage *msg);

		void handleReadAllGraphicNames();
		void handleReadAllMainCategories();

//		void handleChangeFeaturedCategory(SessionMessage *msg);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //CATEGORYMANAGER_H
