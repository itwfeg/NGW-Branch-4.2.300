/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef TEXTLINKS_H
#define TEXTLINKS_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"
#include "Product.h"

class DBAccess;

#include "DBQuery.h"
#include "DBProduct.h"


class TextLinks : public HobList
{
	public:

		TextLinks();
		TextLinks(DBAccess *dbAccess, ProdSupportTextType textType, void *dbRecord);

		~TextLinks();

		ProdSupportTextType getTextType() const;

		edb::DBQuery<edb::DBProduct>* getLinkList();

//		const char *serialize(int format) const;


	private:

		ProdSupportTextType textType;		//	Expanded Text, Special Message, etc.
		
		edb::DBQuery<edb::DBProduct>* linkList;

		void init();


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //TEXTLINKS_H
