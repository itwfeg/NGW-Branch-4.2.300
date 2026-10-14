/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ALLCOOLTEXTS_H
#define ALLCOOLTEXTS_H

#include "evolution.h"
#include "HobList.h"
#include "HfcObject.h"

class DBAccess;

class AllCoolTexts : public HfcObject
{
	public:

		AllCoolTexts();
		AllCoolTexts(const char *data, int len, int formatType);
		~AllCoolTexts();

		void findAll(DBAccess *dbAccess);
		void findAllLinkedToProduct(DBAccess *dbAccess);

		const char *serialize(int format = BrowserFormat) const;


	private:

		HobRecordNum	productNum;
		bool			addTextsIfMissing;		//	Add the Current & Default Text nums to the list if they are not there. This will not save to the db.

		HobList			recordNumList;
		HobList			textList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif //ALLCOOLTEXTS_H
