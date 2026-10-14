/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTTEXTCFGTAGS_H
#define PRODUCTTEXTCFGTAGS_H

#include "Tags.h"

class ProductTextCfgTags : public Tags
{
	public:
		ProductTextCfgTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~ProductTextCfgTags();

		static const std::string textType;
		static const std::string printerNum;
		static const std::string labelNum;

	protected:
		const TagsList &getMyTagsList() const {return productTextCfgTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList productTextCfgTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//PRODUCTTEXTCFGTAGS_H 
