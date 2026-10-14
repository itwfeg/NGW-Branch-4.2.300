/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGOPERATORACCESS_H
#define CFGOPERATORACCESS_H

#include "evolution.h"
#include "CfgBase.h"
#include "HfcObject.h"

#include <list>


/**
 * These are notes copied from CRC card:
 *
 * Business Rule: access level of 1 has no access to supervisor functions.
 *
 * what functions allowed(ACCESS_CODE)
 * LANGUAGE
 * Supervisor Functions
 * Service Functions
 *
 * The comments above are from the very initial Dominator design effort.
 *
 * This class is used to maintain what the operator can modify.
 * This data is stored with configuration (somewhere).
 *
 * This class was created so that we can configure what each operator access level can modify,
 * allowing the manager to set up each operator access level individually.
 * This is not a requirement.  The functionality required is the same as provided by the Ultima
 * (DPS-5.0.0.PP).  The data will be stored with configuration, but will still be loaded into this class
 * to allow flexibility in the future to enhance this functionality, by operator access level.
 *
 * Having this separate class will not adversely affect implementing this feature.
 */
class CfgOperatorAccess : public CfgBase, public HfcObject
{
	public:

		CfgOperatorAccess();
		CfgOperatorAccess(const char *data, int len, int formatType);
		~CfgOperatorAccess();


		void updateConfiguration() const;
		bool isSaveNeeded() const;

		virtual const char *serialize(int format) const;

		static void initFieldList();


	private:
	
		static std::list<CfgFieldOffsets> cfgOAFieldList_;

		// Do not allow the default copy constructor.
		CfgOperatorAccess(const CfgOperatorAccess&);
		// Do not allow the default assignment operator.
		CfgOperatorAccess& operator=(const CfgOperatorAccess&);


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGOPERATORACCESS_H
