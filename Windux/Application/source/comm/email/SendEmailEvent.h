#ifndef SENDEMAILEVENT_H
#define SENDEMAILEVENT_H



#include "EvolMessage.h"


#include <string>
#include "BaseNT.h"


//class HobThread;


class SendEmailEvent : public EvolMessage
{
	public:
		SendEmailEvent(const std::string& path, EvolDeviceType deviceType = EmailManagerDev, UINT8 eventType = SendEmailEvent);
		SendEmailEvent(const char *buffer, int sizeOfMessage);		
		~SendEmailEvent();

		const std::string& getPath() {return path_;}
		void setPath(const std::string& newPath) {path_ = newPath;}


		const char *serialize();
		void show();


	private:
		std::string path_;
};



#endif	//SENDEMAILEVENT_H

