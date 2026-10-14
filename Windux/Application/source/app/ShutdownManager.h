/*
 * ShutdownManager.h
 *
 *  Created on: Jan 9, 2013
 *      Author: aaron
 */

#ifndef SHUTDOWNMANAGER_H
#define SHUTDOWNMANAGER_H

#include "CEvolMsgReceiver.h"
#include "CSmartPtr.h"

#if defined (__linux__)
#include <linux/input.h>
#endif

class ShutdownManager : public Hobart::CEvolMsgReceiver, public Util::CRunnableClass
{

public:
	static ShutdownManager *getInstance();

	~ShutdownManager();


private:
	int fd;
	std::string shutdownButton;

#if defined (__linux__)
	struct input_event inputEvent;
#endif

	bool shutdownEvent;

	static ShutdownManager *pInstance;

	ShutdownManager();
	bool init();
	Util::CSmartPtr<Util::CTLSThread>	shutdownManagerThread;
	virtual DWORD Run();

	virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );



};






#endif
