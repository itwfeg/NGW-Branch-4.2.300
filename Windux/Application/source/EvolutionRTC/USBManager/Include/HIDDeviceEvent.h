/*
 * HIDDevceEvent.h
 *
 *  Created on: Jan 29, 2013
 *      Author: aaron swift
 */

#ifndef HIDDEVICEEVENT_H
#define HIDDEVICEEVENT_H
#include "EvolMessage.h"


class HIDDeviceEvent : public EvolMessage
{

	public:

	typedef enum
	{
		UNKOWN_HIDEVENT,
		MOUSE_ATTACHED,
		MOUSE_REMOVED,
		KEYBOARD_ATTACHED,
		KEYBOARD_REMOVED,
		BARCODE_SCANNER_ATTACHED,
		BARCODE_SCANNER_REMOVED,
		STORAGE_DEVICE_ATTACHED,
		STORAGE_DEVICE_REMOVED
	}HIDEvent;

	HIDDeviceEvent(Hobart::CEvolMsgReceiver *pHobThread = NULL, HIDEvent event = UNKOWN_HIDEVENT);
	~HIDDeviceEvent();
	HIDDeviceEvent(const char *buffer);

	void setEvent(unsigned int evt);
	HIDEvent getHidEvent() const;
	const char *serialize() const;


	private:
	HIDEvent event;

#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};


#endif
