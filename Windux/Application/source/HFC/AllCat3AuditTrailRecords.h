/* HT-Series Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#ifndef ALLCAT3AUDITTRAILRECORDS_H
#define ALLCAT3AUDITTRAILRECORDS_H

#include "evolution.h"
#include "HfcObject.h"

#include "WeigherWriteCat3Record.h"
#include <vector>

class Cat3AuditTrailRecord;


class AllCat3AuditTrailRecords : public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		AllCat3AuditTrailRecords();
		~AllCat3AuditTrailRecords();

		const char *serialize(int format = BrowserFormat) const;

		void addCat3AuditTrailRecord(Cat3AuditTrailRecord *rec);
		int getRecordCount() const;
		void createReportString(std::string &reportString, bool reportEntireAuditTrail = false);
		void saveAuditTrailToDisk(std::string filename, bool exportEntireAuditTrail = false);
		void formatWeigherAuditValue( std::string &valStr, SealableParamID param, unsigned long value ) const;


	private:

		std::vector<Cat3AuditTrailRecord *> cat3AuditTrailRecords;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //ALLCAT3AUDITTRAILRECORDS_H
