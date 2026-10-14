
#include "LabelApplierLabelApplierKind.h"
#include "ILAManager.h"

#include "ILACycle.h"
#include "ILAForceLabel.h"
#include "ILAOpenWindow.h"
#include "ILACloseWindow.h"
#include "ILAOff.h"
#include "ILARequestSpeed.h"
#include "ILASpeed.h"

#include "WeigherRequestInquire.h"
#include "WeigherInquire.h"
#include "WeigherConfiguration.h"

#include "Debug.h"
#include "HobString.h"
#include "Hex.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ILAManager::dataType = 0;
std::string ILAManager::dataTypeName("ILAManager"); 
#endif

ILAManager::ILAManager() 
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	sendCloseWindow = false;
	sendForceLabel = false;
	sendOff = false;

	ticksPerSecond = 100; 
	ilaMaxCycleTime = 10 * ticksPerSecond; //Why? Because Ultima did it this way!!!

	previousTicks = 0;
	ilaPackagesPerMinute = 0;
	halfCycleTime = 0;

	registerEvent(LabelApplier, LaILAReqSpeed);
	registerEvent(Weigher, WgInquire); 

	LabelApplierLabelApplierKind laKind(this, ILA);
	dispatchEvent(&laKind);

	WeigherRequestInquire wri( this );
	dispatchEvent(&wri);
}

ILAManager::~ILAManager()
{
	deregisterAllEvents();

	//shutdown the msg thread
	ShutdownNicely();

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void ILAManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
	Util::CLockCriticalSection lock( ilaManagerCS );

	if ( msg.get() != NULL )
	{
		switch (msg->getDeviceType())
		{
			case LabelApplier	: handleLabelApplierEvent((LaEvent) msg->getEventType(), msg->Data());
								  break;
			case Weigher		: handleWeigherEvent((WgEvent) msg->getEventType(), msg->Data());
				   				  break;
			default				:
				   				  break;
		}
	}
		

	if (sendCloseWindow == true)
	{
		Debug::trace("IlaManager", "ILACloseWindow");

		ILACloseWindow ilaCloseWindow( this );
		dispatchEvent(&ilaCloseWindow);

		sendCloseWindow = false;

		// approx. 90% of duty cycle
		createTimer( halfCycleTime/3 * 10, ilaSendForceLabel );
	}		
	else if (sendForceLabel == true)
	{
		Debug::trace("IlaManager", "ILAForceLabel");

		ILAForceLabel ilaForce(this);
		dispatchEvent(&ilaForce);

		sendForceLabel = false;

		createTimer( ilaMaxCycleTime, ilaSendOff );
	}		
	else if (sendOff == true)
	{
		Debug::trace("IlaManager", "ILAOff");

		ILAOff ilaOff(this);
		dispatchEvent(&ilaOff);

		sendOff = false;

		// reset the Cycle window!
		previousTicks = 0;
		ilaPackagesPerMinute = 0;
		halfCycleTime = 0;

		ILASpeed ilaSpeed( this, ilaPackagesPerMinute);
		dispatchEvent(&ilaSpeed);

	}		
}

void ILAManager::handleLabelApplierEvent(LaEvent laEvent, const char * /* recvBuffer */)
{
	switch (laEvent)
	{
		case LaILACycle:
						{
							Debug::trace("IlaManager.Message", "LaILACycle");
							cancelTimer(ilaSendForceLabel);
                        #if defined(LINUX_FIX)
							ULONG currentTicks = PAL_absclass::PAL_getTickCount();
                        #else
							ULONG currentTicks = ::GetTickCount();
                        #endif
							if (previousTicks != 0)
							{
								ULONG elapsedTicks = currentTicks - previousTicks;
								UINT16 currentSpeed = (UINT16) (60.0 * (double) ticksPerSecond / (double) elapsedTicks);

								if ((ilaPackagesPerMinute == 0) || (currentSpeed != ilaPackagesPerMinute))
								{
									ilaPackagesPerMinute = currentSpeed;

									ILASpeed ilaSpeed(this, ilaPackagesPerMinute);
									dispatchEvent(&ilaSpeed);

									Debug::trace("IlaManager.Stats", HobString("ILAManager -- ticksPerSecond: ") +
																	 HobString(Integer::toString(ticksPerSecond)));
									Debug::trace("IlaManager.Stats", HobString("ILAManager -- currentTicks: ") +
																	 HobString(Integer::toString(currentTicks)));
									Debug::trace("IlaManager.Stats", HobString("ILAManager -- previousTicks: ") +
																	 HobString(Integer::toString(previousTicks)));
									Debug::trace("IlaManager.Stats", HobString("ILAManager -- elapsedTicks: ") +
																	 HobString(Integer::toString(elapsedTicks)));
									Debug::trace("IlaManager.Stats", HobString("ILAManager -- halfCycleTime: ") +
																	 HobString(Integer::toString(halfCycleTime)));
									Debug::trace("IlaManager.Stats", HobString("ILAManager -- ilaPackagesPerMinute: ") +
																	 HobString(Integer::toString(ilaPackagesPerMinute)));
								}

								// approx. 60% of duty cycle
								halfCycleTime = (int) ((double) (elapsedTicks) * 0.60);
								createTimer( halfCycleTime * 10, ilaSendCloseWindow ); 
							}

							previousTicks = currentTicks;

							ILAOpenWindow ilaOpenWindow ( this );
							dispatchEvent(&ilaOpenWindow);

							ILACycle ilaCycle(this);
							dispatchEvent(&ilaCycle);
						}
						break;
		case LaILAReqSpeed:
						{
							Debug::trace("IlaManager.Message", "LaILAReqSpeed");

							ILASpeed ilaSpeed( this, ilaPackagesPerMinute);
							dispatchEvent(&ilaSpeed);
						}
						break;
		default			:
						{
							Debug::trace("IlaManager.Message", "Some Other LaEvent:" + HobString(Hex::toString((UINT16) laEvent)));
						}
						break;
	}
}

void ILAManager::handleWeigherEvent(WgEvent wgEvent, const char *recvBuffer)
{
	switch (wgEvent)
	{
		case WgInquire: {
							WeigherInquire weigherInquire(recvBuffer);
							weigherInquire.show();
							if (weigherInquire.getPrepackMotionCount() != 2)
							{
								Debug::trace("IlaManager.WeigherPrepackCout", "Setting PrepackCount to 2");

								weigherInquire.setPrepackMotionCount(2);
								weigherInquire.setConfigType(WgPermanent);
								weigherInquire.setEventType(WgConfig);
								dispatchEvent(&weigherInquire);
							}
						}
						break;
    #if defined(LINUX_FIX)
        default:
        {
            // Do nothing.
            break;
        }
    #endif
	}
}

void ILAManager::timeoutProcess( __int32 timer_id )
{
	Util::CLockCriticalSection lock( ilaManagerCS );

	if ( timer_id == ilaSendCloseWindow )
		sendCloseWindow = true;
	else if ( timer_id == ilaSendForceLabel )
		sendForceLabel = true;
	else if ( timer_id == ilaSendOff )
		sendOff = true;

	Util::CSmartPtr< EvolMessageWrapper>  msg( NULL );
	processEvent( msg );	
}

/*
int ILAManager::ilaSendCloseWindow(void *pObject)
{
	ILAManager *pIlaManager = (ILAManager *) pObject;

	pIlaManager->sendCloseWindow = true;

	return 0;
}

int ILAManager::ilaSendForceLabel(void *pObject)
{
	ILAManager *pIlaManager = (ILAManager *) pObject;

	pIlaManager->sendForceLabel = true;

	return 0;
}

int ILAManager::ilaSendOff(void *pObject)
{
	ILAManager *pIlaManager = (ILAManager *) pObject;

	pIlaManager->sendOff = true;

	return 0;
}*/
