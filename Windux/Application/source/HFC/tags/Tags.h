/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef TAGS_H
#define TAGS_H

#include <string>
#include <vector>
#include <list>
#include <map>

#include "PAL_platformheader.h" 

typedef std::map<std::string, UINT16> TagsMap;
typedef std::vector<std::string> TagsList;
typedef std::list<UINT16> FieldOffsetsList;
typedef FieldOffsetsList::iterator FieldOffsetsListIterator;


class Tags
{
	public:
		static const std::string emptyTag;
		static const std::string taskNameTag;  // Used by SessionDev; put this in here because I didn't want to create a SessionTags class for just this tag, wasn't worth the effort.
		static const std::string hookActionTag;  // Used by SessionDev
		static const std::string hookRequestTag;  // Used by SessionDev

		Tags();
		virtual ~Tags();

		const std::string &getTag(unsigned int index) const;
		UINT16 getOffset(const char *tag) const;
		UINT16 getOffset(const std::string tag) const;

		void toString() const;

	protected:
		virtual const TagsList &getMyTagsList() const = 0;
		virtual TagsMap &getMyTagsMap() const = 0;

		void putTagsInMap(FieldOffsetsList *pFieldList);

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //TAGS_H
