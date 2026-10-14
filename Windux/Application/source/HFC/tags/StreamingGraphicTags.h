/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef STREAMINGGRAPHICTAGS_H
#define STREAMINGGRAPHICTAGS_H

#include "Tags.h"

class StreamingGraphicTags : public Tags
{
	public:
		StreamingGraphicTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~StreamingGraphicTags();

	protected:
		const TagsList &getMyTagsList() const;
		TagsMap &getMyTagsMap() const;

	private:
		static const std::string streamingGraphicNum;
		static const std::string nextStreamingGraphicNum;
		static const std::string imageFilename;
		static const std::string displayTime;
		static const std::string actionType;
		static const std::string actionParameter;

		static TagsList streamingGraphicTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//STREAMINGGRAPHICTAGS_H 
