/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PERIPHERALTAGS_H
#define PERIPHERALTAGS_H

#include "Tags.h"

class PeripheralTags : public Tags
{
	public:
		PeripheralTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~PeripheralTags();

		static const std::string type;
		static const std::string subType;
		static const std::string kind;
		static const std::string serialNumber;
		static const std::string installationDate;
		static const std::string serviceHistory;
		static const std::string hardwareVersion;
		static const std::string softwareVersion;
		static const std::string checkingPresence;

	protected:
		const TagsList &getMyTagsList() const {return peripheralTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList peripheralTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//PERIPHERALTAGS_H 
