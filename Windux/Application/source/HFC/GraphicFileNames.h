/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef GRAPHICFILENAMES_H
#define GRAPHICFILENAMES_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"
#include "Graphic.h"
#include "filesys/directory.h"
#include <list>

class GraphicFileNames : public HobList
{
	public:

		GraphicFileNames(short graphicType = Graphic::TYPE_LABELGRAPHIC, short graphicSubType = Graphic::SUBTYPE_USERANDSYSTEM, const std::string* rootDir = 0);
		GraphicFileNames(const char *data, int len, int formatType);

		~GraphicFileNames();

		void findFileNames(short graphicType, short graphicSubType, const std::string* rootDir, Directory::Sortby sort = Directory::NAME_SORT);

		virtual const char *serialize(int format) const;

		const util_filesystem::Filename &getMyFilename() const;
		const util_filesystem::Filename &getMyFilename2() const;
		const util_filesystem::Filename &getDestFilename() const;
		const util_filesystem::Filename &getDestFilename2() const;

		bool isSerializeModifiedDateTimes() const { return serializeModifiedDateTimes; }
		void setSerializeModifiedDateTimes(bool serializeModified) { serializeModifiedDateTimes = serializeModified; }

	private:

		util_filesystem::Filename myFilename;
		util_filesystem::Filename myFilename2;
		util_filesystem::Filename destFileName;
		util_filesystem::Filename destFileName2;

		bool serializeModifiedDateTimes;

		HobList modifiedDateTimesList;


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //GRAPHICFILENAMES_H
