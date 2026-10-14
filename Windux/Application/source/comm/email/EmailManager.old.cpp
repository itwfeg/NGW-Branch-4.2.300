#include "EmailManager.h"



#include "EmailManagerEvents.h"



std::string EmailManager::_ComponentName_ = "EmailMgr";



EmailManager::EmailManager(bool startNow)
		: ComponentManager(_ComponentName_, startNow)
{
	registerListener(EmailManagerDev, SendEmail);
}


EmailManager::~EmailManager()
{
	deregisterAllListeners();
}


int EmailManager::handleMessage(EvolMessage* message)
{
}

