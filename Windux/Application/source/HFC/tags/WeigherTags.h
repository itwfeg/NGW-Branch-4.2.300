/* HTSeries Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef WEIGHERTAGS_H
#define WEIGHERTAGS_H

#include "Tags.h"


class WeigherTags : public Tags
{
	public:
		WeigherTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~WeigherTags();

	protected:
		const TagsList &getMyTagsList() const;
		TagsMap &getMyTagsMap() const;

	private:
//		Primary Printer settings
		static const std::string weigherModel;	
		static const std::string weigherFilterSpeed;	
		static const std::string weigherScaleFactor;	
		static const std::string weigherMaxWeight;	
		static const std::string weigherMinWeightToPrint;	
		static const std::string weigherMode;	

		static TagsList weigherTags;
		static TagsMap  fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//WEIGHERTAGS_H
