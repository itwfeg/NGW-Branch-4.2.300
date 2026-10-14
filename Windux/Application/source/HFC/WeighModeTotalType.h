/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef WEIGHMODETOTALTYPE_H
#define WEIGHMODETOTALTYPE_H

#include "evolution.h"
#include "OpModeEvent.h"
#include "HfcObject.h"


namespace hfc
{
	class TransactionType;
}


class WeighModeTotalType : public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		WeighModeTotalType();
		WeighModeTotalType(OpModeEvent runMode, hfc::TransactionType* runTransactionType);
		WeighModeTotalType(const char *data, int len, int formatType);
		~WeighModeTotalType();

		virtual const char *serialize(int format) const;

		OpModeEvent getRunWeighMode();
		void setRunWeighMode(OpModeEvent runWeighMode);


	private:

		// Defined in Event.h
		OpModeEvent runWeighMode;
		hfc::TransactionType* runModeTransactionType;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //WEIGHMODETOTALTYPE_H
