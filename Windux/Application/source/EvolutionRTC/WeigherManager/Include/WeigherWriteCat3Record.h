/**
 * @file WeigherWriteCat3Record.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief Message instructs the weigher to write cat3 record to audit trail.
 * Created on: July 1, 2016
 */

#ifndef WEIGHERWRITECAT3RECORD_H_
#define WEIGHERWRITECAT3RECORD_H_
#include "EvolMessage.h"
#include "Weigher.h"
#include "CEvolMsgReceiver.h"

class WeigherWriteCat3Record : public EvolMessage
{
	public:

		WeigherWriteCat3Record(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		WeigherWriteCat3Record(const char *buffer);
		~WeigherWriteCat3Record();

		AuditEvent getEvent()  					{ return event; }
		void setEvent(AuditEvent ev) 			{ event = ev; }

		SealableParamID getParamId() 			{ return id; }
		void setParamId(SealableParamID param)	{ id = param; }

		AuditType getAuditType()				{ return auditType; }
		void setAuditType(AuditType type)		{ auditType = type; }

		unsigned long getEpochTime()			{ return epochTime; }
		void setEpochTime(unsigned long time)	{ epochTime = time; }

		unsigned long getNewValue()				{ return newValue; }
		void setNewValue(unsigned long value)	{ newValue = value; }

		unsigned long getPreviousValue()				{ return previousValue; }
		void setPreviousValue(unsigned long value)		{ previousValue = value; }


		const char *serialize() const;

		void show();


	private:
		AuditEvent event;
		SealableParamID id;
		AuditType auditType;
		unsigned long epochTime;
		unsigned long newValue;
		unsigned long previousValue;
};
#endif
