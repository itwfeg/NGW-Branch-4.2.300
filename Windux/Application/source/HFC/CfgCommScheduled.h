/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGCOMMSCHEDULED_H
#define CFGCOMMSCHEDULED_H

#include "evolution.h"
#include "CfgBase.h"
#include "HobIpAddress.h"

#include <list>


class CfgCommScheduled : public CfgBase, public HfcObject
{
	public:

		CfgCommScheduled();
		CfgCommScheduled(const char *data, int len, int formatType);
		~CfgCommScheduled();

		void updateConfiguration() const;
		bool isSaveNeeded() const;

		virtual const char *serialize(int format) const;

		static void initFieldList();

		void show();


	private:

		static std::list<CfgFieldOffsets> basicCommScheduledConfigList_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGCOMMSCHEDULED_H
