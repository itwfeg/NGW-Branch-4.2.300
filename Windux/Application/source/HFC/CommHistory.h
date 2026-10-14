/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COMMHISTORY_H
#define COMMHISTORY_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobDateTime.h"
#include <vector>
class DBAccess;


class CommHistory : public HfcObject
{
	public:
		CommHistory();
		~CommHistory();

		void recordComm(EvolDeviceType deviceType, UINT8 eventType);
		void saveCommHistory(DBAccess *dbAccess);

		virtual const char *serialize(int format) const;

	private:

		std::vector<EvolDeviceType> deviceEventList;
		std::vector<int> recordCountList;

		HobDateTime commSessionStart;

		void init();
		bool alreadyReceived(EvolDeviceType deviceType) const;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //COMMHISTORY_H
