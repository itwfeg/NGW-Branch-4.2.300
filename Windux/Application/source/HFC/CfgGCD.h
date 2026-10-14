/* Evolution Scale Project
 * COPYRIGHT (c) 2013.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGGCD_H
#define CFGGCD_H

#include "evolution.h"
#include "CfgBase.h"
#include "HfcObject.h"
#include "GCD.h"

#include <list>



class CfgGCD :	public CfgBase, public HfcObject
{
	public:

		CfgGCD();
		CfgGCD(const char *data, int len, int formatType);
		~CfgGCD();


		void updateConfiguration() const;
		bool isSaveNeeded() const;

		virtual const char *serialize(int format) const;
		void getFieldAsString(int fieldCode, HobString &result, int format) const;

		static void initFieldList();


	private:

		static std::list<CfgFieldOffsets> cfgGCDFieldList_;

		// Do not allow the default copy constructor.
		CfgGCD(const CfgGCD&);
		// Do not allow the default assignment operator.
		CfgGCD& operator=(const CfgGCD&);


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGGCD_H
