/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CLASSTREEBRANCH_H
#define CLASSTREEBRANCH_H

#include "evolution.h"
#include "RecordTreeBranch.h"

class ClassTreeBranch : public RecordTreeBranch
{
	public:

		ClassTreeBranch();
		ClassTreeBranch(HobRecordNum classNum, const std::string& description, edb::DBQuery<edb::DBClass>* classesLinkedTo);

		~ClassTreeBranch();


		virtual const char *serialize(int format) const;



	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif //CLASSTREEBRANCH_H




