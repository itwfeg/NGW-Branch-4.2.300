/* HT-Series Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#ifndef HFCWEIGHERCAT3STATISTICS_H
#define HFCWEIGHERCAT3STATISTICS_H

#include "evolution.h"
#include "WeigherCat3Statistics.h"
#include "HfcObject.h"


class HfcWeigherCat3Statistics : public WeigherCat3Statistics, public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HfcWeigherCat3Statistics(const HfcWeigherCat3Statistics&);
		HfcWeigherCat3Statistics();
		/* Don't think that you can just provide default data for len and formatType
		 * in the second constructor below to provide the functionality for the next
		 * constructor.  These two need to call different constructor for their parent
		 * class, WeigherStatus.
		 */
		HfcWeigherCat3Statistics(const char *data);
		HfcWeigherCat3Statistics(const char *data, int len, int formatType);
		HfcWeigherCat3Statistics(const HfcWeigherCat3Statistics *hws);

		~HfcWeigherCat3Statistics();

		virtual const char *serialize(int format) const;


	private:

		void init();

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //HFCWEIGHERCAT3STATISTICS_H
