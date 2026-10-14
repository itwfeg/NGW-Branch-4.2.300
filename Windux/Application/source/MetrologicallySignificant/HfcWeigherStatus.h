/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWEIGHERSTATUS_H
#define HFCWEIGHERSTATUS_H

#include "evolution.h"
#include "WeigherStatus.h"
#include "HfcObject.h"


class HfcWeigherStatus : public WeigherStatus, public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HfcWeigherStatus(const HfcWeigherStatus&);
		HfcWeigherStatus();
		/* Don't think that you can just provide default data for len and formatType
		 * in the second constructor below to provide the functionality for the next
		 * constructor.  These two need to call different constructor for their parent
		 * class, WeigherStatus.
		 */
		HfcWeigherStatus(const char *data);
		HfcWeigherStatus(const char *data, int len, int formatType);
		HfcWeigherStatus(const HfcWeigherStatus *hws);

		~HfcWeigherStatus();

		bool isWeightValidForDisplaying() const;
		bool isStableWeight() const;
		bool isWeigherInError() const;

		HobWeight getWeight() const;

		const HobString &getWeightDisplay() const;

		void determineDisplayValues() const;

		virtual const char *serialize(int format) const;

		bool operator==(HfcWeigherStatus &hws) const;
//		void operator=(HfcWeigherStatus *hws);

	private:

		mutable HobString weightDisplay;

		void init();

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //HFCWEIGHERSTATUS_H
