/* Evolution Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef BACKUPRESTOREINITIALEREADALLMSG_H
#define BACKUPRESTOREINITIALEREADALLMSG_H

#include "CEvolMsgReceiver.h"
#include "EvolMessage.h"
#include "HfcObject.h"

class CommMechanism;

class BackupRestoreInitiateReadAllMsg : public EvolMessage, public HfcObject
{
	public:
		BackupRestoreInitiateReadAllMsg(Hobart::CEvolMsgReceiver *pHobThread = 0, EvolDeviceType dev = UnknownDevice, UINT8 ev = 0, CommMechanism *pUi = 0);
		BackupRestoreInitiateReadAllMsg(const char *buffer);
		~BackupRestoreInitiateReadAllMsg();

		virtual const char *serialize() const;
		virtual const char *serialize(int format) const;

		EvolDeviceType getReadAllDevice();
		void setReadAllDevice(EvolDeviceType dev);

		UINT8 getReadAllEvent();
		void setReadAllEvent(UINT8 ev);

		CommMechanism *getUiClient();
		void setCommMechanism(CommMechanism *ui);

		void show();

	private:
		EvolDeviceType readAllDevice;
		UINT8 readAllEvent;
		CommMechanism *uiClient;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //BACKUPRESTOREINITIALEREADALLMSG_H
