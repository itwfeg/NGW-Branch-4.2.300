#include "SendEmailEvent.h"



SendEmailEvent::SendEmailEvent(const std::string& path, EvolDeviceType deviceType, UINT8 eventType)
		: EvolMessage(deviceType, eventType)
{
}
