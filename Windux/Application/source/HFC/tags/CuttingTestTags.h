/* Access Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CUTTINGTESTTAGS_H
#define CUTTINGTESTTAGS_H

#include "Tags.h"


class CuttingTestTags : public Tags
{
	public:
		CuttingTestTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~CuttingTestTags();

		static const std::string cuttingTestNum;
		static const std::string primalProductNum;
		static const std::string startDateTime;
		static const std::string endDateTime;
		static const std::string lotNumber;
		static const std::string packingDate;
		static const std::string packingHours;
		static const std::string packingMinutes;
		static const std::string comments;
		static const std::string finalComments;
		static const std::string status;
		static const std::string percentTotalShrinkage;
		static const std::string totalWeightOfCutProducts;
		static const std::string totalPriceOfCutProducts;


	protected:
		const TagsList &getMyTagsList() const {return cuttingTestTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList cuttingTestTags;
		static TagsMap  fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};


#endif //CUTTINGTESTTAGS_H
