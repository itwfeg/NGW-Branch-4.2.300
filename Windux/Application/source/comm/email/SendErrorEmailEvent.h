#ifndef SENDERROREMAILEVENT_H
#define SENDERROREMAILEVENT_H



#include "EvolMessage.h"
//#include "HobThread.h"

#include <string>


class SendErrorEmailEvent : public EvolMessage
{
	public:
		// Types and subtypes
		typedef enum ErrorEmailType
		{
			ErrorEmailTypeError,
			ErrorEmailTypeResolved,
			ErrorEmailTypeGcdPlaylistError,
			ErrorEmailTypeGcdFtpError,

		} ErrorEmailType_t;

		typedef enum ErrorEmailCode
		{
			ErrorEmailCodeOutOfLabels,
			ErrorEmailCodeCassetteOpen,
			ErrorEmailCodeGcdPlaylistError,
			ErrorEmailCodeGcdFtpError,
			ErrorEmailCodeNetworkOffline,
			ErrorEmailCodeLabelPresent,
			ErrorEmailFPPUpdateFailed,

			EndErrorEmailCode				//	All new errors go before this one.
		} ErrorEmailCode_t;

		SendErrorEmailEvent(Hobart::CEvolMsgReceiver *pHobThread = 0);
		// Constructors and destructor
		SendErrorEmailEvent(ErrorEmailType type, ErrorEmailCode code, Hobart::CEvolMsgReceiver *pHobThread = 0);
		SendErrorEmailEvent(const char *buffer);
		~SendErrorEmailEvent();

		ErrorEmailType getType() const { return type_; }
		void setType(ErrorEmailType type) { type_ = type; }
		ErrorEmailCode getCode() const { return code_; }
		void setCode(ErrorEmailCode code) { code_ = code; }

		const char *serialize() const;
		void show();


	private:
		ErrorEmailType type_;
		ErrorEmailCode code_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif	//SENDERROREMAILEVENT_H

