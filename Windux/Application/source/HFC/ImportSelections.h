/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef IMPORTSELECTIONS_H
#define IMPORTSELECTIONS_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobString.h"


typedef enum ISelectionFieldOffsets
{
	fieldOffsetIDirName = 1,
	fieldOffsetIMoveFile,
	endISelectionFieldOffsets
} ISelectionFieldOffsets_t;
class ImportSelections : public HfcObject
{
	public:

		ImportSelections();
		ImportSelections(const char *data, int len, int formatType);

		~ImportSelections();

		HobString getDirName() const;
		void setDirName(HobString dirName);

		bool getMoveFile() const;
		void setMoveFile(bool moveIt);

		virtual const char *serialize(int format = BrowserFormat) const;

	private:

		HobString dirName_;
		bool moveFile_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif //IMPORTSELECTIONS_H
