/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef FEATUREDCATEGORY_H
#define FEATUREDCATEGORY_H

#include "evolution.h"
#include "HfcObject.h"

typedef enum FeaturedCategoryFieldOffsets
{
	fieldOffsetDisplayPosition = 1,
	fieldOffsetNewCategoryNum,
	fieldOffsetOldCategoryNum,
	endFeaturedCategoryFieldOffsets
}   FeaturedCategoryFieldOffsets_t;


class FeaturedCategory : public HfcObject
{
	public:

		FeaturedCategory();
		FeaturedCategory(const char *data, int len, int formatType);

		~FeaturedCategory();

		int getDisplayOrder() const;
		void setDisplayOrder(int order);

		HobRecordNum getNewCategoryNum() const;
		void setNewCategoryNum(HobRecordNum num);

		HobRecordNum getOldCategoryNum() const;
		void setOldCategoryNum(HobRecordNum num);


	private:

		int	displayOrder;
		HobRecordNum newCategoryNum;
		HobRecordNum oldCategoryNum;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //FEATUREDCATEGORY_H




