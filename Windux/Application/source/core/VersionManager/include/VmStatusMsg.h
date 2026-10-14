#ifndef VMSTATUSMSG_H
#define VMSTATUSMSG_H

#include "EvolMessage.h"
#include "VersionManagerEvent.h"
#include "HfcResponse.h"

//class HobThread;


class VmStatusMsg : public EvolMessage
{
	public:
		VmStatusMsg(Hobart::CEvolMsgReceiver *pHobThread = NULL,
					VersionManagerDevError statusCode = VersionManagerErrUnknownError,
					UINT16 responseType = HfcResponse::RESPONSESTYLE_NODIALOG,
					bool removeDialog = false);
		VmStatusMsg(const char *buffer);
		~VmStatusMsg();

		const char *serialize() const;
		void show();

		VersionManagerDevError getStatusCode();
		void setStatusCode(VersionManagerDevError statusCode);

		UINT16 getResponseType();
		void setResponseType(UINT16 responseType);

		bool isRemoveDialog();
		bool getRemoveDialog();
		void setRemoveDialog(bool removeDialog = true);
		void clearRemoveDialog();

	private:
		VersionManagerDevError statusCode;
		UINT16 responseType;
		bool removeDialog;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif	//VMSTATUSMSG_H
