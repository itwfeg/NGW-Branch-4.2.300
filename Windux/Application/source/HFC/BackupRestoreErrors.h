/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef BACKUPRESTOREERRORS_H
#define BACKUPRESTOREERRORS_H

#include "evolution.h"
#include "HfcObject.h"

class HobList;


class BackupRestoreErrors : public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		BackupRestoreErrors();
		~BackupRestoreErrors();

		HobList *getFileIndicatorsList();
		void setFileIndicatorsList(HobList *fileIndicatorsList);

		HobList *getErrorRecordsList();
		void setErrorRecordsList(HobList *errorRecordsList);

		const char *serialize(int format = BrowserFormat) const;


	private:

		HobList *fileIndicatorsList;
		HobList *errorRecordsList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //BACKUPRESTOREERRORS_H
