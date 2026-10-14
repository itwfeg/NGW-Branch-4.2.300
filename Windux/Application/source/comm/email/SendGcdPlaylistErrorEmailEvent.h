#ifndef SENDGCDPLAYLISTERROREMAILEVENT_H
#define SENDGCDPLAYLISTERROREMAILEVENT_H



#include "EvolMessage.h"
//#include "HobThread.h"

#include "HobString.h"


class SendGcdPlaylistErrorEmailEvent : public EvolMessage
{
public:

		SendGcdPlaylistErrorEmailEvent(Hobart::CEvolMsgReceiver *pHobThread = 0);
		// Constructors and destructor
		SendGcdPlaylistErrorEmailEvent(HobString& msg, Hobart::CEvolMsgReceiver *pHobThread = 0);
		SendGcdPlaylistErrorEmailEvent(HobString& msg, HobString& url, Hobart::CEvolMsgReceiver *pHobThread = 0);
		SendGcdPlaylistErrorEmailEvent(const char *buffer, int length);
		~SendGcdPlaylistErrorEmailEvent();

		const char *serialize() const;
		void show();

		HobString& getErrorMessage() { return err_msg;}
		HobString& getFileUrl() { return file_url;}

private:
	HobString err_msg;
	HobString file_url;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};


#endif	//SENDGCDPLAYLISTERROREMAILEVENT_H

