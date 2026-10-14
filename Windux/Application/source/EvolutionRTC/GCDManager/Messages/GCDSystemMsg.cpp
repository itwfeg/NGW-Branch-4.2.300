#include "GCDSystemMsg.h"

GCDSystemMsg::GCDSystemMsg(EvolDeviceType deviceType, UINT8 eventType, const HobString& data) :
	EvolMessage(deviceType, eventType), m_data(data)
{
}

GCDSystemMsg::~GCDSystemMsg(void)
{
}

const char *GCDSystemMsg::serialize() const
{
	EvolMessage::serialize();

	serializedData.append(m_data);

	return serializedData.data();
}