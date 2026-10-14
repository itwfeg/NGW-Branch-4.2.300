/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SOFTKEYFEATURE_H
#define SOFTKEYFEATURE_H

#include "evolution.h"
#include "HfcObject.h"
#include "SoftkeyBasics.h"

#include <list>
namespace edb
{
class DBSoftKeyFeature;
}
class DBAccess;


typedef enum SoftkeyFeatureFieldOffsets
{
	fieldOffsetSKFNumber = 1,
	fieldOffsetSKFHorizontalSpacing,
	fieldOffsetSKFVerticalSpacing,
	fieldOffsetSKFDefSoftkeyGroup,
	fieldOffsetSKFSoftkeyStyle,
	fieldOffsetSKFVerticalGridSize,
	fieldOffsetSKFHorizontalGridSize,
	fieldOffsetSKFDeptNum,
	fieldOffsetSKFFixedRowColMethod,
	fieldOffsetSKFDefaultButton,
	endSoftkeyFeatureFieldOffsets
} SoftkeyFeatureFieldOffsets_t;


class SoftkeyFeature : public HfcObject
{
	public:

		SoftkeyFeature(DBAccess *access = 0);
		SoftkeyFeature(const char *data, int len, int formatType);
		SoftkeyFeature(edb::DBSoftKeyFeature *dbSoftkeyRec, DBAccess *access = (DBAccess *)NULL);

		~SoftkeyFeature();

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum deptNum);

		static void initFieldLists();

		void setDBAccess(DBAccess *dbAccess);

		edb::DBSoftKeyFeature *getMyDbRecord();

		bool isRecordValid();

		HobRecordNum getFeatureNumber() const;
		void setFeatureNumber(HobRecordNum number);

		int getDefaultHorizontalGridSize() const;
		void setDefaultHorizontalGridSize(int size);

		int getDefaultVerticalGridSize() const;
		void setDefaultVerticalGridSize(int size);

		int getHorizontalSpacing() const;
		void setHorizontalSpacing(int spacing);

		int getVerticalSpacing() const;
		void setVerticalSpacing(int spacing);

		FixedRowColMethod getFixedRowColMethod() const;
		void setFixedRowColMethod(FixedRowColMethod frcm);

		int getDefaultButton() const;
		void setDefaultButton(int defButton);

		HobRecordNum getDefaultGroupNumber() const;
		void setDefaultGroupNumber(HobRecordNum groupNum);

		HobRecordNum getStyleNumber() const;
		void setStyleNumber(HobRecordNum styleNum);

		int updateField(int fieldCode, const HobString &token);
		void update(const SoftkeyFeature *newSoftkey);
		void getFieldAsString(int fieldCode, HobString &result, int format) const;

		static SoftkeyFeature *find(HobRecordNum recordNum, HobRecordNum deptNum, DBAccess *dbAccess);

		int deleteRecord();

		void saveRecord();

		virtual const char *serialize(int format) const;

		static void verifyReservedRecords(DBAccess *dbAccess);


	private:
		HobRecordNum		recordNumber;
		int					defaultHGridSize;
		int					defaultVGridSize;
		int					horizontalSpacing;
		int					verticalSpacing;
		FixedRowColMethod	fixedRowColMethod;
		int					defaultButton;
		HobRecordNum		defaultGroupNumber;
		HobRecordNum		styleNumber;
		HobRecordNum		deptNum;

		DBAccess		 *dbAccess;
		edb::DBSoftKeyFeature *myDbRecord;

		void init();

		static void createReservedFeatureRecord(DBAccess *dbAccess, HobRecordNum deptNum, HobRecordNum featureNum, int hSize, int vSize, int hSpacing, int vSpacing);

		static std::list<SoftkeyFeatureFieldOffsets> browserFieldList, softkeyGroupList, evolFieldList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //SOFTKEYFEATURE_H
