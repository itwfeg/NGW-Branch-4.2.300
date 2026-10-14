/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ALLPRODUCTTEXTCFGS_H
#define ALLPRODUCTTEXTCFGS_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"

class DBAccess;

class AllProductTextCfgs : public HfcObject
{
	public:

		AllProductTextCfgs();
		~AllProductTextCfgs();

		void findAll(DBAccess *dbAccess);

		const char *serialize(int format = BrowserFormat) const;


	private:

		HobList productTextNumsList;
		HobList productTextWhichPrinterList;
		HobList productTextLabelNumList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //ALLPRODUCTTEXTCFGS_H
