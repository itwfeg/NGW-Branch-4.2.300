/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTTREEBRANCH_H
#define PRODUCTTREEBRANCH_H

#include "evolution.h"
#include "RecordTreeBranch.h"

class ProductTreeBranch : public RecordTreeBranch
{
	public:

		ProductTreeBranch();
		ProductTreeBranch(HobRecordNum productNum, edb::DBQuery<edb::DBProduct>* productsLinkedTo);
		ProductTreeBranch(HobRecordNum productNum, const std::string& description, edb::DBQuery<edb::DBProduct>* productsLinkedTo);

		~ProductTreeBranch();

		virtual const char *serialize(int format) const;



	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif //PRODUCTTREEBRANCH_H




