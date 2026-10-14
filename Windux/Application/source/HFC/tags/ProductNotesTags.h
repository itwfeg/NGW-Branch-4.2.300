/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTNOTESTAGS_H
#define PRODUCTNOTESTAGS_H

#include "Tags.h"

class ProductNotesTags : public Tags
{
	public:
		ProductNotesTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~ProductNotesTags();

		static const std::string productNotesTextNumber;
		static const std::string productNotesTextText;

	protected:
		const TagsList &getMyTagsList() const {return productNotesTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList productNotesTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//PRODUCTNOTESTAGS_H
