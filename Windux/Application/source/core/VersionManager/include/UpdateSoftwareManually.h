#ifndef UPDATESOFTWAREMANUALLY_H
#define UPDATESOFTWAREMANUALLY_H
//#include "vxWorks.h"
#include "EvolMessage.h"
//#include "HobThread.h"
#include "CEvolMsgReceiver.h"
#include <string>

class UpdateSoftwareManually : public EvolMessage
{
	public:
		UpdateSoftwareManually(std::string& path, Hobart::CEvolMsgReceiver *pHobThread = NULL);
		UpdateSoftwareManually(const char *buffer, int sizeOfMessage);		
		~UpdateSoftwareManually();

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



#endif	//UPDATESOFTWAREMANUALLY_H
