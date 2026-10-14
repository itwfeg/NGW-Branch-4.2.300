/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef RECORDTREEBRANCH_H
#define RECORDTREEBRANCH_H

#include "evolution.h"
#include "HobList.h"
#include "DBQuery.h"
#include "DBClass.h"
#include "DBProduct.h"


class RecordTreeBranch : public HobList
{
	public:

		RecordTreeBranch();
		RecordTreeBranch(HobRecordNum classNum, const std::string& description, edb::DBQuery<edb::DBClass>* classesLinkedTo);
		RecordTreeBranch(HobRecordNum productNum, const std::string& description, edb::DBQuery<edb::DBProduct>* productsLinkedTo);

		~RecordTreeBranch();

		bool hasLinks();
		HobRecordNum getRecordNum() const;


	protected:

		mutable HobString serializedData;

	private:

		long	junkParameter;	//	So I can see all of the private parameters.

		HobRecordNum	recordNum;
		std::string			description;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //RECORDTREEBRANCH_H




