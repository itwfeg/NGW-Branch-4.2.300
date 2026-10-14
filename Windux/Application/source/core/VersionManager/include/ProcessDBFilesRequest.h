#ifndef PROCESSDBFILESREQUEST_H
#define PROCESSDBFILESREQUEST_H
#include "EvolMessage.h"
#include <string>
#include "CEvolMsgReceiver.h"
//class HobThread;


class ProcessDBFilesRequest : public EvolMessage
{
	public:
		ProcessDBFilesRequest(const std::string& dbDir, Hobart::CEvolMsgReceiver *pHobThread = NULL);
		ProcessDBFilesRequest(const char *buffer);
		~ProcessDBFilesRequest();

		const std::string& getDBDirectory(){return tmpDBDir_;}

		const char *serialize() const;
		void show();

	private:
		std::string tmpDBDir_;
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif	//PROCESSDBFILESREQUEST_H
