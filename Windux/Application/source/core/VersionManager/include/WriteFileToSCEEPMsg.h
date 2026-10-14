#ifndef WRITEFILETOSCEEPMSG_H
#define WRITEFILETOSCEEPMSG_H
//#include "vxWorks.h"
#include "EvolMessage.h"
//#include "HobThread.h"

#include <string>

class WriteFileToSCEEPMsg : public EvolMessage
{
	public:
		WriteFileToSCEEPMsg(std::string& path, Hobart::CEvolMsgReceiver *pHobThread = NULL);
		WriteFileToSCEEPMsg(const char *buffer, int sizeOfMessage);		
		~WriteFileToSCEEPMsg();

		const std::string& getPath() {return path_;}
		void setPath(const std::string& newPath) {path_ = newPath;}
		const char *serialize() const;
		void show();

	private:
		std::string path_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};



#endif	//WRITEFILETOSCEEPMSG_H
