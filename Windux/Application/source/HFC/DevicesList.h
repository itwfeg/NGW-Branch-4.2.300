/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef DEVICESLIST_H
#define DEVICESLIST_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"
#include <list>

class DevicesList : public HobList
{
	public:

		DevicesList();
		DevicesList(const char *data, int len, int formatType);

		~DevicesList();

		void findDeviceNames();


	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif // DEVICESLIST_H
