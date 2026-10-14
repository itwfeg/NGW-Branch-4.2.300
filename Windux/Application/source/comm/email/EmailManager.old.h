#ifndef EMAILMANAGER_H
#define EMAILMANAGER_H



#include "EmailComponent.h"
#include "core/ComponentManager.h"
#include "EvolMessage.h"
#include THREAD_INC



class EmailManager : public ComponentManager
{
	public:
		EmailManager(bool startNow = true);
		~EmailManager();


	protected:
		int handleMessage(EvolMessage* message);


	private:


		static const std::string _ComponentName_;
};



#endif // EMAILMANAGER_H
