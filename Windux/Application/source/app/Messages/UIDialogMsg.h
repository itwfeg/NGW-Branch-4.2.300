#ifndef UIDIALOGMSG_H
#define UIDIALOGMSG_H

#include "EvolMessage.h"
#include "SessionEvent.h"
#include "HfcResponse.h"


class UIDialogMsg : public EvolMessage
{
	public:
		UIDialogMsg(Hobart::CEvolMsgReceiver *pHobThread = NULL,
						int errorDevice = 0,
						int statusCode = 0,
						UINT16 responseType = HfcResponse::RESPONSESTYLE_NODIALOG,
						bool removeDialog = false);
		UIDialogMsg(const char *buffer);
		~UIDialogMsg();

		const char *serialize() const;
		void show();

		int getErrorDevice();
		void setErrorDevice(int device);

		int getStatusCode();
		void setStatusCode(int code);

		UINT16 getResponseType();
		void setResponseType(UINT16 type);

		bool isRemoveDialog();
		void setRemoveDialog(bool remove = true);

	private:
		int errorDevice;
		int statusCode;
		UINT16 responseType;
		bool removeDialog;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//UIDIALOGMSG_H
