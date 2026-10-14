/* Evolution Scale Project
 * COPYRIGHT (c) 2009.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGACCUMULATEITEMS_H
#define CFGACCUMULATEITEMS_H

#include "evolution.h"
#include "CfgBase.h"
#include "HfcObject.h"

#include <list>


class CfgAccumulateItems : public CfgBase, public HfcObject
{
	public:

		CfgAccumulateItems();
		CfgAccumulateItems(const char *data, int len, int formatType);
		~CfgAccumulateItems();


		void updateConfiguration() const;
		bool isSaveNeeded() const;

		virtual const char *serialize(int format) const;
		void getFieldAsString(int fieldCode, HobString &result, int format) const;

		static void initFieldList();		


	private:

		static std::list<CfgFieldOffsets> cfgAIFieldList_, browserFieldList_;

 
		// Do not allow the default copy constructor.
		CfgAccumulateItems(const CfgAccumulateItems&);
		// Do not allow the default assignment operator.
		CfgAccumulateItems& operator=(const CfgAccumulateItems&);


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGACCUMULATEITEMS_H
