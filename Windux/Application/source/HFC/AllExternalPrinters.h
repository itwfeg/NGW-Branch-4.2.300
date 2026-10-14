/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ALLEXTERNALPRINTERS_H
#define ALLEXTERNALPRINTERS_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"

class DBAccess;

class AllExternalPrinters : public HfcObject
{
	public:

		AllExternalPrinters();
		~AllExternalPrinters();

		void findAll(DBAccess *dbAccess);

		const char *serialize(int format = BrowserFormat) const;


	private:

		HobList extPrinterNumList;
		HobList extPrinterUserNameList;
		HobList extPrinterTypeList;
		HobList extPrinterSystemNameList;
		HobList extPrinterIpAddressList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //ALLEXTERNALPRINTERS_H
