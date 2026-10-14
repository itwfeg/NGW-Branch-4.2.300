/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SOFTKEYSTYLE_H
#define SOFTKEYSTYLE_H

#include "evolution.h"
#include "HfcObject.h"
#include "SoftkeyBasics.h"

#include <list>
namespace edb
{
class DBSoftKeyStyle;
}
class DBAccess;


typedef enum SoftkeyStyleFieldOffsets
{
	fieldOffsetSKSStyleNumber = 1,
	fieldOffsetSKSTextVerticalAlignment,
	fieldOffsetSKSTextColor,
	fieldOffsetSKSGrayedOutTextColor,
	fieldOffsetSKSBackgroundColor,
	fieldOffsetSKSBackgroundImage,
	fieldOffsetSKSImageDirectory,
	fieldOffsetSKSForegroundImageLocation,
	endSoftkeyStyleFieldOffsets
} SoftkeyStyleFieldOffsets_t;


typedef enum ImageLocation
{
	upperLeft,
	upperCenter,
	upperRight,
	centerLeft,
	center,
	centerRight,
	lowerLeft,
	lowerCenter,
	lowerRight
} ImageLocation_t;


class SoftkeyStyle : public HfcObject
{
	public:

		SoftkeyStyle(DBAccess *access = 0);
		SoftkeyStyle(const char *data, int len, int formatType);
		SoftkeyStyle(edb::DBSoftKeyStyle *dbSoftkeyRec, DBAccess *access = (DBAccess *)NULL);

		~SoftkeyStyle();

		static void initFieldLists();

		void setDBAccess(DBAccess *dbAccess);

		edb::DBSoftKeyStyle *getMyDbRecord();

		bool isRecordValid();

		HobRecordNum getStyleNumber() const;
		void setStyleNumber(HobRecordNum number);

		VerticalAlignment getTextVerticalAlignment() const;
		void setTextVerticalAlignment(VerticalAlignment position);

		const HobString &getTextColor() const;
		void setTextColor(const HobString &color);

		const HobString &getGrayedOutTextColor() const;
		void setGrayedOutTextColor(const HobString &color);

		const HobString &getBackgroundColor() const;
		void setBackgroundColor(const HobString &color);

		const HobString &getBackgroundImage() const;
		void setBackgroundImage(const HobString &imageName);

		const HobString &getImageDirectory() const;
		void setImageDirectory(const HobString &dirName);

		AreaPosition getForegroundImageLocation() const;
		void setForegroundImageLocation(AreaPosition location);

		int updateField(int fieldCode, const HobString &token);
		void update(SoftkeyStyle *newSoftkey);
		void getFieldAsString(int fieldCode, HobString &result, int format) const;

		static SoftkeyStyle *find(HobRecordNum styleNum, DBAccess *dbAccess);

		int deleteRecord();

		void saveRecord();

		virtual const char *serialize(int format) const;

		void show();

		static void verifyReservedRecords(DBAccess *dbAccess);


	private:
		HobRecordNum		styleNumber;
		VerticalAlignment	textVerticalAlignment;
		HobString			textColor;
		HobString			grayedOutTextColor;
		HobString			backgroundColor;
		HobString			backgroundImage;
		HobString			imageDirectory;
		AreaPosition		foregroundImageLocation;

		DBAccess		*dbAccess;
		edb::DBSoftKeyStyle	*myDbRecord;

		void init();

		static std::list<SoftkeyStyleFieldOffsets> browserFieldList, softkeyGroupList, evolFieldList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //SOFTKEYSTYLE_H
