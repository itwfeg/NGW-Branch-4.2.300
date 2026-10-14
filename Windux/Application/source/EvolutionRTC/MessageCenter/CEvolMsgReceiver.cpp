// CEvolMsgReceiver.cpp: implementation of the CEvolMsgReceiver class.
//
//////////////////////////////////////////////////////////////////////

#include "CEvolMsgReceiver.h"
#include "CMessageCenter.h"
#include "SessionManager.h"
#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

namespace Hobart
{
#ifdef USE_ACCOUNTANT
    int CEvolMsgReceiver::CMessageCheckingThread::dataType = 0;
    std::string CEvolMsgReceiver::CMessageCheckingThread::dataTypeName("CMessageCheckingThread");
#endif

    CEvolMsgReceiver::CMessageCheckingThread::CMessageCheckingThread( CEvolMsgReceiver * receiver , bool managedThread,
            bool superThread, bool startIt) : msgReceiver ( receiver )

    {
#ifdef USE_ACCOUNTANT
        if ( dataType == 0 )
            dataType = util_acc::registerDataType( dataTypeName );

        util_acc::addInstance( dataType );
#endif
        std::string name = msgReceiver->getReceiverName();

		int priority = THREAD_PRIORITY_NORMAL;			//	 0

		// Added by Carlos to fix pause while printing with Atmel Printer
		if(name == "PrinterManager" || name == "PrinterCommManager")
			priority = THREAD_PRIORITY_TIME_CRITICAL;	//	15

		if ( name.size() )
			name.append(1, ' ');

		name.append("EvolReceiver's Checking Thread");

		checkingThread = new Util::CTLSThread( *this, priority, 0, name.c_str(), false, managedThread, superThread);

	    if (checkingThread && startIt)
	            RestartThread();
//        Debug::trace( "CEvolMsgReceiver", "CMessageCheckingThread():  Done.  this = " + util_string::ptrtostring(this));
    }

    CEvolMsgReceiver::CMessageCheckingThread::~CMessageCheckingThread()
    {
#ifdef USE_ACCOUNTANT
        util_acc::removeInstance(dataType);
#endif
        if ( checkingThread )
        {
            checkingThread->StopThread();
            checkingThread->WaitForCompletion();
        }

        //tell the thread to exit
        m_ExitEvent.EventSet();

        //wait for the thread to exit before deleting all the resources!
        if ( checkingThread )
        {
            while ( checkingThread->IsRunning() )
                PAL_absclass::PAL_sleep( 1 );

            // this will delete thread object
            checkingThread = NULL;
        }
    }

    void CEvolMsgReceiver::CMessageCheckingThread::RequestToShutdownNicely()
    {
        Debug::trace( "CEvolMsgReceiver", "shutdown nicely requested.  this = " + util_string::ptrtostring(this));

        // stop the thread
        if ( checkingThread )
            checkingThread->StopThread();

        // tell the thread to exit
        m_ExitEvent.EventSet();

        if ( checkingThread )
        {
        	int waitForCompletionTime = 250;
        	int elapsed = 0;

        	// wait patiently for thread to exit
        	while (( checkingThread->IsRunning() ) && ( !checkingThread->WaitForCompletion( waitForCompletionTime ) ))
            {
                PAL_absclass::PAL_sleep( 1 );
                elapsed += waitForCompletionTime;

                if ( 5000 < elapsed )
                    Debug::trace( "CEvolMsgReceiver", "still waiting for thread completion..." );
            }
        }

        Debug::trace( "CEvolMsgReceiver", "Shutdown nicely request complete!  this = " + util_string::ptrtostring(this));
    }

    DWORD CEvolMsgReceiver::CMessageCheckingThread::Run()
    {
        DWORD retVal = 1;

        try
        {
			Util::CMSGSemaphore *sem( &msgReceiver->msgStream.GetNewMessSemaphore() );

			// wait for new message
			while ( !m_ExitEvent.IsEventSet() )
				if ( WAIT_OBJECT_0 == sem->Dec() )
				{
                    Util::CSmartPtr< EvolMessageWrapper > p_message;

                   	// retrieve message
					msgReceiver->msgStream >> p_message;

                    // process message
                    if (( NULL != p_message.get() ) && ( !m_ExitEvent.IsEventSet() ))
                        msgReceiver->processEvent( p_message );
                }

        }
        catch(Util::CTerminateNow)
        {
			Debug::trace("CEvolMsgReceiver", HobString("Run():  catch(Util::CTerminateNow)"));

            if (checkingThread->IsThreadManaged())
                SessionManager::getInstance()->deRegisterThread(checkingThread->GetThreadID(), checkingThread->IsASuperThread());
			Debug::trace("CEvolMsgReceiver", HobString("Run():  catch(Util::CTerminateNow)   return 1;"));
            
			return 1;
        }
        catch(...)
        {
			Debug::trace("CEvolMsgReceiver.error", HobString("Run():  catch(...)"));

            if (checkingThread->IsThreadManaged())
			{
                SessionManager::getInstance()->deRegisterThread(checkingThread->GetThreadID(), checkingThread->IsASuperThread());
				Debug::trace("CEvolMsgReceiver.error", HobString("Run():  catch(...)   deRegisterThread()"));
			}

            LogCatch;
            retVal = -1;
			Debug::trace("CEvolMsgReceiver.error", HobString("Run():  catch(...)   return -1;"));
        }
		Debug::trace("CEvolMsgReceiver", "CMessageCheckingThread run loop is exiting.  this = " + util_string::ptrtostring(this) + ",  retVal = " + util_string::longtostring(retVal));

		return retVal;
    }

#ifdef USE_ACCOUNTANT
    int CEvolMsgReceiver::dataType = 0;
    std::string CEvolMsgReceiver::dataTypeName("CEvolMsgReceiver");
#endif

    CEvolMsgReceiver::CEvolMsgReceiver( const char * name , bool managedThread, bool superThread, bool startIt) : receiverName( name )
    {
#ifdef USE_ACCOUNTANT
        if (dataType == 0)
            dataType = util_acc::registerDataType(dataTypeName);

        util_acc::addInstance(dataType);
#endif

        try
        {
            msgCheckingThread = Util::CSmartPtr< CMessageCheckingThread >( new CMessageCheckingThread( this , managedThread, superThread, startIt) );
        }
        catch (...)
        {
			Debug::trace("CEvolMsgReceiver.error", HobString("CEvolMsgReceiver(") + HobString(name) + HobString("):  catch(...)"));
            LogCatchAndThrow;
        }
//		Debug::trace("CEvolMsgReceiver", HobString("CEvolMsgReceiver():  receiverName = [") + HobString(getReceiverName()) + "],  this = " + util_string::ptrtostring(this));
    }
    CEvolMsgReceiver::~CEvolMsgReceiver()
    {
		try
        {
			deregisterAllEvents();            
        }
        catch(...)
        {
			Debug::trace("CEvolMsgReceiver.error", HobString("~CEvolMsgReceiver():  catch(...),  receiverName = [") + HobString(getReceiverName()) + "]");
        }

#ifdef USE_ACCOUNTANT
        util_acc::removeInstance(dataType);
#endif
        if (msgCheckingThread)
			msgCheckingThread->RequestToShutdownNicely();
//		Debug::trace("CEvolMsgReceiver", HobString("~CEvolMsgReceiver():  receiverName = [") + HobString(getReceiverName()) + "],  this = " + util_string::ptrtostring(this));
    }

    void CEvolMsgReceiver::registerEvent( EvolDeviceType evol_device, UINT8 evol_event, bool permanent )
    {
        //Debug::trace("CEvolMsgReceiver.registerEvent", "registerEvent device: " + evol_device);  // + HobString("   event: " + inttostring(evol_event)
		//Debug::trace("CEvolMsgReceiver.registerEvent", "registerEvent event: " + util_string::inttostring(evol_event));

		CMessageCenter::instance().registerEvent( evol_device, evol_event , this, permanent );
    }

    void CEvolMsgReceiver::deregisterEvent( EvolDeviceType evol_device, UINT8 evol_event )
    {
        CMessageCenter::instance().deregisterEvent( evol_device, evol_event, this );
    }

    void CEvolMsgReceiver::deregisterAllEvents()
    {
        CMessageCenter::instance().deregisterAllEvents( this );
    }

    void CEvolMsgReceiver::dispatchEvent( const EvolMessage * msg, CEvolMsgReceiver * receiver )
    {
        if ( msg != NULL )
            CMessageCenter::instance().dispatchEvent( msg, receiver );
    }

    void CEvolMsgReceiver::dispatchEvent( const char * msg, int length, CEvolMsgReceiver * receiver )
    {
        if ( msg != NULL && length > 0 )
            CMessageCenter::instance().dispatchEvent( msg, length, receiver );
    }
    void CEvolMsgReceiver::send( EvolMessage * msg )
    {
        if ( msg != NULL )
        {
            Util::CSmartPtr<EvolMessageWrapper>
                msg_wrapper_ptr(new EvolMessageWrapper(msg));
            addEvolMessage(msg_wrapper_ptr);
        }
    }

    void CEvolMsgReceiver::send( const char * ptr, uint32_t length )
        {
            if ( ptr != NULL && length > 0 )
            {
                Util::CSmartPtr<EvolMessageWrapper>
                    msg_wrapper_ptr(new EvolMessageWrapper(ptr, length));
                addEvolMessage(msg_wrapper_ptr);
            }
        }


    void CEvolMsgReceiver::addEvolMessage( Util::CSmartPtr< EvolMessageWrapper > & msg )
    {
    	// NGW-259: prevent crash when tracing from tracing thread during exception
    	static bool withinException( false );

    	if ( withinException )
    		return;

        try
        {
            msgStream << msg;
        }
        catch(Util::CTerminateNow)
        {
        	withinException = true;
			Debug::trace("CEvolMsgReceiver.error", "addEvolMessage():  catch(Util::CTerminateNow),  receiverName = [" + getReceiverName() + "]");

            return;
        }
        catch (...)
        {
        	withinException = true;
			Debug::trace("CEvolMsgReceiver.error", "addEvolMessage():  catch(...),  receiverName = [" + getReceiverName() + "]");

            LogCatchAndThrow;
        }
    }

    //          template Util::CSmartPtr<EvolMessageWrapper> d;
    //          void CEvolMsgReceiver::addEvolMessage(Util::CSmartPtr<EvolMessageWrapper>  &msg);
    //template<class EvolMessageWrapper> class CSemStream<EvolMessageWrapper >;
}// end of namespace



