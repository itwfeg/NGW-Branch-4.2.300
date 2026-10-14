/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGFREQUENTSHOPPER_H
#define CFGFREQUENTSHOPPER_H

#include "evolution.h"
#include "CfgBase.h"
#include "HfcObject.h"

#include <list>


class CfgFrequentShopper : public CfgBase, public HfcObject
{
	public:

		CfgFrequentShopper();
		CfgFrequentShopper(const char *data, int len, int formatType);
		~CfgFrequentShopper();


		void updateConfiguration() const;
		bool isSaveNeeded() const;

		virtual const char *serialize(int format) const;
		bool isFieldQuoted(int fieldOffsetCfg) const;

		void validateDefaultDiscountValues();

		static void initFieldList();


	private:

		static std::list<CfgFieldOffsets> cfgFSFieldList_, browserFieldList_;

 
		// Do not allow the default copy constructor.
		CfgFrequentShopper(const CfgFrequentShopper&);
		// Do not allow the default assignment operator.
		CfgFrequentShopper& operator=(const CfgFrequentShopper&);


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGFREQUENTSHOPPER_H
