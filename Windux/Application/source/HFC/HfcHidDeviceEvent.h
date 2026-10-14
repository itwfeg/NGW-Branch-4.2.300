#ifndef HFCHIDDEVICEEVENT_H
#define HFCHIDDEVICEEVENT_H
#include "HfcObject.h"
#include "HIDDeviceEvent.h"

class HfcHidDeviceEvent : public HIDDeviceEvent, public HfcObject
{
	
	public:
	HfcHidDeviceEvent();
	HfcHidDeviceEvent(const char *data, int len, int formatType);
	HfcHidDeviceEvent(const char *data);
	virtual const char *serialize(int format = InternalFormat) const;

	HIDEvent getHidDeviceEvent() const;
	
	private:
	
		
	
};
#endif
