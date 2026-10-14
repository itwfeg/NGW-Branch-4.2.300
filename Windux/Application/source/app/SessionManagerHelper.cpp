#include "SessionManagerHelper.h"
#include "SessionManager.h"

SessionManagerHelper::SessionManagerHelper()
{
}

SessionManagerHelper::~SessionManagerHelper()
{
}

SessionManagerHelper *SessionManagerHelper::instance = (SessionManagerHelper *)NULL;

SessionManagerHelper *SessionManagerHelper::getInstance()
{
    if (instance == 0)
        instance = new SessionManagerHelper();
    return instance;
}

void SessionManagerHelper::panicShutdown()
{
	SessionManager::getInstance()->panicShutdown();
}


void SessionManagerHelper::addDialog(int errorDevice, int errorCode, const std::string msgData)
{
        Debug::trace("SessionManager", "SMH::addDialog before send to SessionManager");
	SessionManager::getInstance()->addDialog(errorDevice, errorCode, msgData);
        Debug::trace("SessionManager", "SMH::addDialog after send to SessionManager");
}

void SessionManagerHelper::removeDialog(int errorDevice, int errorCode)
{
        Debug::trace("SessionManager", "SMH::removeDialog before send to SessionManager");
	SessionManager::getInstance()->removeDialog(errorDevice, errorCode);
        Debug::trace("SessionManager", "SMH::removeDialog after send to SessionManager");
}

bool SessionManagerHelper::hasPrimarySession()
{
	return SessionManager::getInstance()->getPrimaryConsoleSession() != NULL;
}

bool SessionManagerHelper::isWeigherAttached()
{
	return SessionManager::getInstance()->isWeigherAttached();
}

bool SessionManagerHelper::isModeCorrectForApplyingTareSetback()
{
	return SessionManager::getInstance()->applyTareSetback();
}
