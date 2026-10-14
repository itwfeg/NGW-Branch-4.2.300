/*
 * HIDDeviceEvent.cpp
 *
 *  Created on: Jan 29, 2013
 *      Author: aaron
 */
#include "HIDDeviceEvent.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HIDDeviceEvent::dataType = 0;
std::string HIDDeviceEvent::dataTypeName = "HIDDeviceEvent";
#endif


HIDDeviceEvent::HIDDeviceEvent(Hobart::CEvolMsgReceiver *pHobThread, HIDEvent event) : EvolMessage(HIDDev, event, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->event = event;
	EvolMessage::setEventType(event);
}

HIDDeviceEvent::~HIDDeviceEvent()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

HIDDeviceEvent::HIDDeviceEvent(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &event;
	for (unsigned int i = 0; i < sizeof(event); i++)
	{
		*pChar++ = buffer[j++];
	}


}
const char *HIDDeviceEvent::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &event;
	for (unsigned int i = 0; i < sizeof(event); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void HIDDeviceEvent::setEvent(unsigned int evt)
{
	event = (HIDDeviceEvent::HIDEvent)evt;
}

HIDDeviceEvent::HIDEvent HIDDeviceEvent::getHidEvent() const
{
	return event;
}
