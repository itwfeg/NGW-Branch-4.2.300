/*
 * ShutdownManager.cpp
 *
 *  Created on: Jan 9, 2013
 *      Author: aaron
 */
#include "ShutdownManager.h"
#include "EvolMessage.h"
#include "SessionEvent.h"
#include "Debug.h"
#include "Integer.h"


/*my instance*/
ShutdownManager *ShutdownManager::pInstance = (ShutdownManager *) NULL;

ShutdownManager *ShutdownManager::getInstance()
{
    if (pInstance == 0)
    {
    	pInstance = new ShutdownManager();
    }

    return pInstance;
}

ShutdownManager::ShutdownManager()
{
	shutdownButton = "/dev/input/event0";
	shutdownEvent = false;
	fd = 0;

	if( init() )
	{
		//create listening thread
		shutdownManagerThread = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "ShutdownManager Thread" );
	}
}


ShutdownManager::~ShutdownManager()
{
	close(fd);

    if (shutdownManagerThread)
	{
    	shutdownManagerThread->StopThread();
    	shutdownManagerThread->WaitForCompletion();
	}
    ShutdownNicely();


}

bool ShutdownManager::init()
{
	bool fileOpen = false;

	if((fd = open(shutdownButton.data(), O_RDONLY)) == -1)
	{
		Debug::trace("ShutdownManager", "Failed to open shutdownButton");
	}
	else
	{
		fileOpen = true;
		Debug::trace("ShutdownManager", "shutdownButton open");
	}
	return fileOpen;
}

DWORD ShutdownManager::Run()
{
	while(!shutdownEvent)
	{
		read(fd, &inputEvent, sizeof(struct input_event));
		if(inputEvent.type == EV_KEY)
		{
			if(inputEvent.value)
			{
				Debug::trace("ShutdownManager", "shutdown event found");
				shutdownEvent = true;

				Debug::trace("ShutdownManager", "sending shutdown request to session manager");
				//send shutdownRequest to the sessionManager
			    EvolMessage msg(SessionDev, SnShutdownRequest);
			    dispatchEvent(&msg);

			}
			else
			{
				Debug::trace("ShutdownManager", "shutdown event: " + Integer::toString(inputEvent.value));
			}
		}
		else
		{
			Debug::trace("ShutdownManager", "Wrong event type: " + Integer::toString(inputEvent.type));
		}
	}
}


void ShutdownManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{

}



