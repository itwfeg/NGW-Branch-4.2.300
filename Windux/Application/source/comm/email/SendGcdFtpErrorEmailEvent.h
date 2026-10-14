#ifndef SENDGCDFTPERROREMAILEVENT_H
#define SENDGCDFTPERROREMAILEVENT_H



#include "EvolMessage.h"
//#include "HobThread.h"

#include "HobString.h"


class SendGcdFtpErrorEmailEvent : public EvolMessage
{
public:

		SendGcdFtpErrorEmailEvent(Hobart::CEvolMsgReceiver *pHobThread = 0);
		// Constructors and destructor
		SendGcdFtpErrorEmailEvent(HobString& msg, Hobart::CEvolMsgReceiver *pHobThread = 0);
		SendGcdFtpErrorEmailEvent(const char *buffer);
		~SendGcdFtpErrorEmailEvent();

		const char *serialize() const;
		void show();

		HobString& getErrorMessage() { return err_msg;}

private:
	HobString err_msg;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif	//SENDGCDFTPERROREMAILEVENT_H

