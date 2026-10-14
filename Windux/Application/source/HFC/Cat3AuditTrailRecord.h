/* HT-Series Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#ifndef CAT3AUDITTRAILRECORD_H
#define CAT3AUDITTRAILRECORD_H

#include "evolution.h"
#include "Weigher.h"
#include "HfcObject.h"
#include <list>


typedef enum Cat3AuditRecordFieldOffsets
{
	fieldOffsetCat3Auditevent = 1,
	fieldOffsetCat3ParameterId,
	fieldOffsetCat3AuditType,
	fieldOffsetCat3EpochTime,
	fieldOffsetCat3NewValue,
	fieldOffsetCat3PreviousValue,
	endCat3AuditRecordFieldOffsets
}   Cat3AuditRecordFieldOffsets_t;


class Cat3AuditTrailRecord : public HfcObject
{
	public:

		Cat3AuditTrailRecord();
		Cat3AuditTrailRecord(const Cat3AuditRecord *data);

		~Cat3AuditTrailRecord();

		AuditEvent getAuditEvent() const { return auditevent; }
		void setAuditevent( AuditEvent ae ) { auditevent = ae; }

		SealableParamID getParameterId() const { return parameterId; }
		void setParameterId( SealableParamID param ) { parameterId = param; }

		AuditType getAuditType() const { return auditType; }
		void setAuditType( AuditType type ) { auditType = type; }

		unsigned long getEpochTime() const { return epochTime; }
		void setEpochTime( unsigned long eTime ) { epochTime = eTime; }

		unsigned long getNewValue() const { return newValue; }
		void setNewValue( unsigned long val ) { newValue = val; }

		unsigned long getPreviousValue() const { return previousValue; }
		void setPreviousValue( unsigned long val ) { previousValue = val; }


		virtual const char *serialize(int format) const;

		void show();

		void saveRecord();

		static void initFieldList();


	private:

		AuditEvent auditevent;
		SealableParamID parameterId;
		AuditType auditType;
		unsigned long epochTime;
		unsigned long newValue;
		unsigned long previousValue;


		static std::list<Cat3AuditRecordFieldOffsets> fieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CAT3AUDITTRAILRECORD_H




