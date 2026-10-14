#ifndef GCDDDEVSIMPLEMSG_H
#define GCDDDEVSIMPLEMSG_H

#include "EvolMessage.h"
#include "HobString.h"
#include "MessageFormat.h"
#include "GCDEvent.h"

class GCDDevNoParamMsg : public EvolMessage
{
public:
	GCDDevNoParamMsg(GCDEvent event);

	UINT32 getSerializedLength() const;

	const char *serialize() const;

protected:
	virtual GCDEvent getGCDEvent() const = 0;
};

class GCDDevShutdownDeviceMsg : public GCDDevNoParamMsg
{
public:
	GCDDevShutdownDeviceMsg() : GCDDevNoParamMsg(GcdShutdownDevice) {}

protected:
	virtual GCDEvent getGCDEvent() const { return GcdShutdownDevice; }
};

class GCDDevGetStatusMsg : public GCDDevNoParamMsg
{
public:
	GCDDevGetStatusMsg() : GCDDevNoParamMsg(GcdGetStatus) {}

protected:
	virtual GCDEvent getGCDEvent() const { return GcdGetStatus; }
};
#endif