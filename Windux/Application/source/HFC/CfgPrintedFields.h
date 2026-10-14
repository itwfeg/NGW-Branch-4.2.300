/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGPRINTEDFIELDS_H
#define CFGPRINTEDFIELDS_H

#include "evolution.h"
#include "CfgBase.h"
#include "HfcObject.h"

#include <list>


/**
 * We may want to determine a different way of handling these fields, so that it is easier in the future
 * to add additional label fields that can be configurably printed.
 *
 * If we allow the application to pass in the 'name' of the field when checking if a field should be printed,
 * and the field name is not in the list, we really don't know if it should be printed or not.  We really only
 * know that the field is not configured in this class, not whether it should be printed or not.
 */
class CfgPrintedFields : public CfgBase, public HfcObject
{
	public:

		CfgPrintedFields();
		CfgPrintedFields(const char *data, int len, int formatType);
		~CfgPrintedFields();


		void updateConfiguration() const;
		bool isSaveNeeded() const;

		virtual const char *serialize(int format) const;

		static void initFieldList();


	private:

		static std::list<CfgFieldOffsets> cfgPFFieldList_;

		// Do not allow the default copy constructor.
		CfgPrintedFields(const CfgPrintedFields&);
		// Do not allow the default assignment operator.
		CfgPrintedFields& operator=(const CfgPrintedFields&);


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGPRINTEDFIELDS_H
