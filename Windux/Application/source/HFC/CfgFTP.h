/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGFTP_H
#define CFGFTP_H

#include "evolution.h"
#include "CfgBase.h"
#include "HobIpAddress.h"

#include <list>


class CfgFTP : public CfgBase, public HfcObject
{
	public:

		CfgFTP();
		CfgFTP(const char *data, int len, int formatType);
		~CfgFTP();


		void updateConfiguration() const;
		bool isSaveNeeded() const;

		virtual const char *serialize(int format) const;
		bool isFieldQuoted(int fieldOffsetCfg) const;

		static void initFieldList();

		void show();


	private:

		static std::list<CfgFieldOffsets> basicFTPConfigList_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGFTP_H
