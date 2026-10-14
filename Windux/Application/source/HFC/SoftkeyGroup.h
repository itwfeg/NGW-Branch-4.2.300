/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SOFTKEYGROUP_H
#define SOFTKEYGROUP_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"
#include "SoftkeyBasics.h"

class DBAccess;

class SoftkeyFeature;
class SoftkeyStyle;
class Softkey;
class Product;
class Special;
class Category;

namespace edb
{
class DBCategory;
class DBSoftKey;
class DBProduct;
class DBSoftKeyGroup;
}

typedef enum SoftkeyGroupFieldOffsets
{
	fieldOffsetSKGGroupNumber = 1,
	fieldOffsetSKGFeatureNumber,
	fieldOffsetSKGHorizontalGridSize,
	fieldOffsetSKGVerticalGridSize,
	fieldOffsetSKGHorizontalSpacing,
	fieldOffsetSKGVerticalSpacing,
	fieldOffsetSKGDeptNum,
	endSoftkeyGroupFieldOffsets
}  SoftkeyGroupFieldOffsets_t;

class SoftkeyGroup : public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		SoftkeyGroup(DBAccess *dbAccess = 0);
		SoftkeyGroup(edb::DBSoftKeyGroup *dbGroupRec, DBAccess *dbAccess);
		SoftkeyGroup(const char *data, int len, int formatType);
		~SoftkeyGroup();

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum deptNum);

		void setDBAccess(DBAccess *dbAccess);

		edb::DBSoftKeyGroup *getMyDbRecord();

		bool isRecordValid();

		HobRecordNum getGroupNumber() const;
		void setGroupNumber(HobRecordNum number);

		SoftkeyFeature *getSoftkeyFeature();
		void setSoftkeyFeature(SoftkeyFeature *newFeature);

		SoftkeyStyle *getSoftkeyStyle();
		void setSoftkeyStyle(SoftkeyStyle *newStyle);

		HobRecordNum getFeatureNumber() const;
		void setFeatureNumber(HobRecordNum number);

		short getCategoryAction();
		void setCategoryAction(short number);

		int getLanguageSelection();
		void setLanguageSelection(int selection);

		bool doesSoftkeyFeatureExist();
		bool doesSoftkeyStyleExist();

		const char *serialize(int format = BrowserFormat) const;

		void gotoFirstPage();
		void gotoNextPage();
		void gotoPreviousPage();
		void setGridCellCount(int count);
		int getGridCellCount() const;

		void add(Softkey &softkey, long featureNum, int languageSelection, int hPos = -1, int vPos = -1, int hSize = -1, int vSize = -1, bool setDefaultValues = true);

		bool addProductSoftkey(edb::DBProduct *dbProductRec, long featureNum, short languageSelection, int hPos, int vPos, int hSize, int vSize, edb::DBCategory *dbCategoryRec = 0, bool allowDuplicates = true);
		void addCategorySoftkey(edb::DBCategory *dbCategoryRec, long featureNum, short languageSelection, int hPos, int vPos, int hSize, int vSize);

		int getHorizontalGridSize() const;
		void setHorizontalGridSize(int size);

		int getVerticalGridSize() const;
		void setVerticalGridSize(int size);

		int getHorizontalSpacing() const;
		void setHorizontalSpacing(int spacing);

		int getVerticalSpacing() const;
		void setVerticalSpacing(int spacing);

		static SoftkeyGroup *find(HobRecordNum recordNum, HobRecordNum deptNum, DBAccess *dbAccess);

		int updateField(int fieldCode, const HobString &token);
		void update(const SoftkeyGroup *newGroup);
		void getFieldAsString(int fieldCode, HobString &result) const;

		int deleteRecord();
		void saveRecord();

		Softkey *getSoftkey(int whichButton);

		static void verifyReservedRecords(DBAccess *dbAccess, bool createDefaultKeys = true);

		void show(const HobString &title);


		void concatenateSoftkeyGroup(SoftkeyGroup *fixedSoftkeyGroup, FixedRowColMethod fixedRowColMethod);
		void updateForNewTopRow(SoftkeyGroup *fixedSoftkeyGroup, int softkeysPerPage);
		void updateForNewBottomRow(SoftkeyGroup *fixedSoftkeyGroup, int softkeysPerPage);
		void updateForNewFirstColumn(SoftkeyGroup *fixedSoftkeyGroup, int softkeysPerPage);
		void updateForNewLastColumn(SoftkeyGroup *fixedSoftkeyGroup, int softkeysPerPage);
		void concatenateLists(SoftkeyGroup *fixedSoftkeyGroup, int softkeysPerPage);
		void concatenateList(HobList &myList, HobList l, int softkeysPerPage);

		HobList gethPosList() const { return hPosList; }
		HobList getvPosList() const { return vPosList; }
		HobList gethSizeList() const { return hSizeList; }
		HobList getvSizeList() const { return vSizeList; }
		HobList getbgColorList() const { return bgColorList; }
		HobList getbgImageList() const { return bgImageList; }
		HobList getfgImageList() const { return fgImageList; }
		HobList getlang1NameList() const { return lang1NameList; }
		HobList getlang2NameList() const { return lang2NameList; }
		HobList getactionList() const { return actionList; }
		HobList getactionParameterList() const { return actionParameterList; }
		HobList getactionFeatureNumList() const { return actionFeatureNumList; }
		HobList getisEnabledList() const { return isEnabledList; }
		HobList gettextColorList() const { return textColorList; }
		HobList getscriptNameList() const { return scriptNameList; }
		HobList getscriptIntervalList() const { return scriptIntervalList; }


	private:

		HobList		hPosList;
		HobList		vPosList;
		HobList		hSizeList;
		HobList		vSizeList;
		HobList		bgColorList;
		HobList		bgImageList;
		HobList		fgImageList;
		HobList		lang1NameList;
		HobList		lang2NameList;
		HobList		actionList;
		HobList		actionParameterList;
		HobList		actionFeatureNumList;
		HobList		isEnabledList;
		HobList		textColorList;
		HobList     scriptNameList;
		HobList     scriptIntervalList;

		HobRecordNum	featureNumber;
		HobRecordNum	groupNumber;
		HobRecordNum    deptNum;

		short			categoryAction;
		SoftkeyFeature	*feature;
		SoftkeyStyle	*style;

		int				languageSelection;
		int				currentSerializeOffset;		//	Support for Self serve categories.
		int				gridCellCount;

		int				horizontalGridSize;
		int				verticalGridSize;
		int				horizontalSpacing;
		int				verticalSpacing;

		DBAccess		*myDbAccess;
		edb::DBSoftKeyGroup	*myDbRecord;

		void init();
		void initFieldLists();

		void getEndOfPath(HobString& result, const HobString& fullPath);

		static void createReservedGroupRecord(DBAccess *dbAccess, HobRecordNum deptNum, HobRecordNum groupNum, int hSize, int vSize, int hSpacing, int vSpacing, bool createDefaultKeys);
			
		static std::list<SoftkeyGroupFieldOffsets> browserFieldList, loadGroupFieldList, evolFieldList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //SOFTKEYGROUP_H
