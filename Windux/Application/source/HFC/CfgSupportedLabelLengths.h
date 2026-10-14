/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGSUPPORTEDLABELLENGTHS_H
#define CFGSUPPORTEDLABELLENGTHS_H

#include "evolution.h"
#include "CfgBase.h"
#include "HfcObject.h"

#include <list>


class CfgSupportedLabelLengths : public CfgBase, public HfcObject
{
	public:

		CfgSupportedLabelLengths();
		CfgSupportedLabelLengths(const char *data, int len, int formatType);
		~CfgSupportedLabelLengths();


		void updateConfiguration() const;
		bool isSaveNeeded() const;

		virtual const char *serialize(int format) const;

		static void initFieldList();


	private:
	
		static std::list<CfgFieldOffsets> cfgSLLFieldList_;

		// Do not allow the default copy constructor.
		CfgSupportedLabelLengths(const CfgSupportedLabelLengths&);
		// Do not allow the default assignment operator.
		CfgSupportedLabelLengths& operator=(const CfgSupportedLabelLengths&);


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGSUPPORTEDLABELLENGTHS_H
