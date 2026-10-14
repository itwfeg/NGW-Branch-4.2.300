/*	Created by Alex Zavala 10/3
*	based off wrapper
*/

#ifndef HFCCLAREMOVEUPGRADEFILES_H
#define HFCCLAREMOVEUPGRADEFILES_H

#include "evolution.h"
#include "HfcObject.h"

#include "CLARemoveUpgradeFiles.h"

class HfcCLARemoveUpgradeFiles : public CLARemoveUpgradeFiles, public HfcObject
{
	public:
		HfcCLARemoveUpgradeFiles();
		HfcCLARemoveUpgradeFiles(const char *data);
		HfcCLARemoveUpgradeFiles(const char *data, int len, int formatType);

		~HfcCLARemoveUpgradeFiles();

		virtual const char *serialize(int format = InternalFormat)const;


	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif	//	HFCCLAREMOVEUPGRADEFILES_H
