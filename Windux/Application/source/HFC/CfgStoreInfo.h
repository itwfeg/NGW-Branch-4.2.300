/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGSTOREINFO_H
#define CFGSTOREINFO_H

#include "evolution.h"
#include "CfgBase.h"
#include "HfcObject.h"

#include <list>



class CfgStoreInfo : public CfgBase, public HfcObject
{
	public:

		CfgStoreInfo();
		CfgStoreInfo(const char *data, int len, int formatType);
		~CfgStoreInfo();

		void updateConfiguration() const;
		bool isSaveNeeded() const;

		virtual const char *serialize(int format) const;

		static void initFieldList();		 		 


	private:

		static std::list<CfgFieldOffsets> browserFieldList_, evolFieldList_, ultimaFieldList_;

		// Do not allow the default copy constructor.
		CfgStoreInfo(const CfgStoreInfo&);
		// Do not allow the default assignment operator.
		CfgStoreInfo& operator=(const CfgStoreInfo&);


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGSTOREINFO_H
