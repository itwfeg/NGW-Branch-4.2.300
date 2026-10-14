/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "FeaturedCategory.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "Integer.h"
#include "Long.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int FeaturedCategory::dataType = 0;
std::string FeaturedCategory::dataTypeName("FeaturedCategory"); 
#endif


FeaturedCategory::FeaturedCategory()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

FeaturedCategory::FeaturedCategory(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetDisplayPosition:
						displayOrder = Integer::valueOf(token);
						break;
					case fieldOffsetNewCategoryNum:
						newCategoryNum = ((HobRecordNum)Long::valueOf(token));
						break;
					case fieldOffsetOldCategoryNum:
						oldCategoryNum = ((HobRecordNum)Long::valueOf(token));
						break;
					default:
						break;
				}
				loopCnt++;
			}
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

FeaturedCategory::~FeaturedCategory()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


//	private
void FeaturedCategory::init()
{
	displayOrder = 0;
	newCategoryNum = (HobRecordNum) RECORD_NOT_FOUND;
	oldCategoryNum = (HobRecordNum) RECORD_NOT_FOUND;
}


int FeaturedCategory::getDisplayOrder() const
{
	return displayOrder;
}
void FeaturedCategory::setDisplayOrder(int order)
{
	displayOrder = order;
}


HobRecordNum FeaturedCategory::getNewCategoryNum() const
{
	return newCategoryNum;
}
void FeaturedCategory::setNewCategoryNum(HobRecordNum num)
{
	newCategoryNum = num;
}


HobRecordNum FeaturedCategory::getOldCategoryNum() const
{
	return oldCategoryNum;
}
void FeaturedCategory::setOldCategoryNum(HobRecordNum num)
{
	oldCategoryNum = num;
}
