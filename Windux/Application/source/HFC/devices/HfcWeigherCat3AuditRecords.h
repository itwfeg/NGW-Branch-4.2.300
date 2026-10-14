/* HT-Series Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#ifndef HFCWEIGHERCAT3AUDITRECORDS_H
#define HFCWEIGHERCAT3AUDITRECORDS_H

#include "evolution.h"
#include "WeigherCat3AuditRecords.h"
#include "HfcObject.h"


class HfcWeigherCat3AuditRecords : public WeigherCat3AuditRecords, public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HfcWeigherCat3AuditRecords(const HfcWeigherCat3AuditRecords&);
		HfcWeigherCat3AuditRecords();
		/* Don't think that you can just provide default data for len and formatType
		 * in the second constructor below to provide the functionality for the next
		 * constructor.  These two need to call different constructor for their parent
		 * class.
		 */
		HfcWeigherCat3AuditRecords(const char *data);
		HfcWeigherCat3AuditRecords(const char *data, int len, int formatType);
		HfcWeigherCat3AuditRecords(const HfcWeigherCat3AuditRecords *hws);

		~HfcWeigherCat3AuditRecords();

		virtual const char *serialize(int format) const;


	private:

		void init();

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //HFCWEIGHERCAT3AUDITRECORDS_H
