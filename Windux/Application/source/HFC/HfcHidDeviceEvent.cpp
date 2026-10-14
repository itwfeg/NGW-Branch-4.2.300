/*
 * HfcHidDeviceEvent.cpp
 *
 *  Created on: Jan 29, 2013
 *      Author: aaron
 */
#include "HfcHidDeviceEvent.h"
#include "HobTokenizerCSV.h"
#include "HfcObject.h"
#include "Integer.h"

HfcHidDeviceEvent::HfcHidDeviceEvent(const char *data, int len, int formatType)
{
    switch (formatType)
    {
        case InternalFormat:
            break;

        case BrowserFormat:
        {
                HobString temp(data, len);
                HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
                HobString token;
                //      New format for this message is as follows:  offset, value
                if (tok.next(&token) != -1)
                	setEvent( Integer::valueOf(token) );
                break;
        }
    }
}

HfcHidDeviceEvent::HfcHidDeviceEvent(const char *data) : HIDDeviceEvent(data)
{

}

const char *HfcHidDeviceEvent::serialize(int format) const
{
	HfcObject::serializedData = "";
	const char *msgdata = NULL;
	int msglen = 0;

	switch (format)
	{
		case InternalFormat:
		{
			msgdata = HIDDeviceEvent::serialize();
			msglen = HIDDeviceEvent::getSerializedLength();

			HfcObject::serializedData = HIDDeviceEvent::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case BrowserFormat:
		{

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString(HIDDeviceEvent::getHidEvent()));
			Debug::trace("UsbManagerLinux.Attach", "HfcObject " + HfcObject::serializedData);
			msgdata = HfcObject::serializedData.data();
			msglen = HfcObject::getSerializedLength();
		}
		break;

		default:
			break;
	}
	return HfcObject::serializedData.data();
}


HIDDeviceEvent::HIDEvent HfcHidDeviceEvent::getHidDeviceEvent() const
{
	return getHidEvent();
}

