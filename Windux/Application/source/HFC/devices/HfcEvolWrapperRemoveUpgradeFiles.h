/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERREMOVEUPGRADEFILES_H
#define HFCEVOLWRAPPERREMOVEUPGRADEFILES_H

#include "evolution.h"
#include "HfcObject.h"

#include "EvolWrapperRemoveUpgradeFiles.h"

class HfcEvolWrapperRemoveUpgradeFiles : public EvolWrapperRemoveUpgradeFiles, public HfcObject
{
	public:
		HfcEvolWrapperRemoveUpgradeFiles();
		HfcEvolWrapperRemoveUpgradeFiles(const char *data);
		HfcEvolWrapperRemoveUpgradeFiles(const char *data, int len, int formatType);

		~HfcEvolWrapperRemoveUpgradeFiles();

		virtual const char *serialize(int format = InternalFormat)const;


	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif	//	HFCEVOLWRAPPERREMOVEUPGRADEFILES_H
