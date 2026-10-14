/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef STATISTICSTAGS_H
#define STATISTICSTAGS_H

#include "Tags.h"

class StatisticsTags : public Tags
{
	public:
		StatisticsTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~StatisticsTags();

	protected:
		const TagsList &getMyTagsList() const;
		TagsMap &getMyTagsMap() const;

	private:
		static const std::string scaleType;
		static const std::string freeFlashMemory; // not in Ultima
		static const std::string totalFlashMemory; // not in Ultima
		static const std::string freeRamMemory;
		static const std::string totalRamMemory;
		static const std::string productsCount;
		static const std::string classesCount;
		static const std::string expandedTextCount;
		static const std::string specialMessagesCount;
		static const std::string productNotesCount; // not in Ultima
		static const std::string nutrifactsCount;
		static const std::string labelTypesCount; // not in Ultima
		static const std::string graphicsCount;
		static const std::string fontsCount;
		static const std::string operatorsCount;
		static const std::string totalsCount;

		static TagsList statisticsTags;
		static TagsMap  fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//STATISTICSTAGS_H
