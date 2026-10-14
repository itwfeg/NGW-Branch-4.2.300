/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGPREPACK_H
#define CFGPREPACK_H

#include "evolution.h"
#include "CfgBase.h"
#include "HfcObject.h"

#include <list>



class CfgPrepack : public CfgBase, public HfcObject
{
	public:

		CfgPrepack();
		CfgPrepack(const char *data, int len, int formatType);
		~CfgPrepack();


		void updateConfiguration() const;
		bool isSaveNeeded() const;

		virtual const char *serialize(int format) const;

		static void initFieldList();		


	private:

		static std::list<CfgFieldOffsets> cfgPPFieldList_, browserFieldList_, rotationOverrideFieldList_;


		// Do not allow the default copy constructor.
		CfgPrepack(const CfgPrepack&);
		// Do not allow the default assignment operator.
		CfgPrepack& operator=(const CfgPrepack&);


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGPREPACK_H
