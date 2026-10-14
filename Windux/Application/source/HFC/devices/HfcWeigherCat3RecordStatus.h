/* HT-Series Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#ifndef HFCWEIGHERCAT3RECORDSTATUS_H
#define HFCWEIGHERCAT3RECORDSTATUS_H

#include "evolution.h"
#include "WeigherCat3RecordStatus.h"
#include "HfcObject.h"


class HfcWeigherCat3RecordStatus : public WeigherCat3RecordStatus, public HfcObject
{
	public:

		HfcWeigherCat3RecordStatus();
		/* Don't think that you can just provide default data for len and formatType
		 * in the second constructor below to provide the functionality for the next
		 * constructor.  These two need to call different constructor for their parent
		 * class.
		 */
		HfcWeigherCat3RecordStatus(const char *data);
		HfcWeigherCat3RecordStatus(const char *data, int len, int formatType);

		~HfcWeigherCat3RecordStatus();

		virtual const char *serialize(int format) const;


	private:

		void init();

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //HFCWEIGHERCAT3RECORDSTATUS_H
