/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef DIRECTORYLIST_H
#define DIRECTORYLIST_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"
#include <list>

class DirectoryList : public HobList
{
	public:

		DirectoryList(HobString startingDirectory);
		DirectoryList(const char *data, int len, int formatType);

		~DirectoryList();

		void findFileNames(HobString location);

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif // DIRECTORYLIST_H
