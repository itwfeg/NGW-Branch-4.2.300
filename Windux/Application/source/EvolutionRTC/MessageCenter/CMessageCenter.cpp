// CMessageCenter.cpp: implementation of the CMessageCenter class.
//
//////////////////////////////////////////////////////////////////////

#include "CMessageCenter.h"
#include "CEvolMsgReceiver.h"

#include "Debug.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

namespace Hobart
{
#ifdef USE_ACCOUNTANT
    int CMessageCenter::dataType = 0;
    std::string CMessageCenter::dataTypeName("CMessageCenter");
#endif

    CMessageCenter*  CMessageCenter::messageCenter = NULL;
    Util::CSmartCriticalSection CMessageCenter::critSec;

    CMessageCenter::CMessageCenter()
    {
#ifdef USE_ACCOUNTANT
        if (dataType == 0)
            dataType = util_acc::registerDataType(dataTypeName);

        util_acc::addInstance(dataType);
#endif

        try
        {
            dispatchThread = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "MessageCenter" );
        }
        catch (...)
        {
	        Debug::trace("CMessageCenter.error", HobString("CMessageCenter():  catch(...)"));
            LogCatchAndThrow;
        }
    }

    CMessageCenter::~CMessageCenter()
    {
#ifdef USE_ACCOUNTANT
        util_acc::removeInstance(dataType);
#endif
        dispatchThread->StopThread();
    }

    CMessageCenter & CMessageCenter::instance()
    {
        try
        {
            if ( NULL == messageCenter )
            {
                Util::CLockCriticalSection lock(  critSec );
                if ( NULL == messageCenter )
                    messageCenter =  new CMessageCenter;
            }
        }
        catch (...)
        {
	        Debug::trace("CMessageCenter.error", HobString("~CMessageCenter():  catch(...)"));
            LogCatchAndThrow;
        }
        return * messageCenter;
    }

    void CMessageCenter::dispatch( Util::CSmartPtr< EvolMessageWrapper > & msg )
    {
        try
        {
            const uint32_t msg_id( ( msg->getDeviceType() << 16 ) + msg->getEventType() );

            Util::CLockCriticalSection lock( critSec );

            //Find all the receivers that register the message and send to them
            //          one by one.
            const std::pair< DispatchMsgMapIterator, DispatchMsgMapIterator> iters( dispatchMsgMap.equal_range( msg_id ) );
            const DispatchMsgMapIterator miterEnd( iters.second );

			// SOF-1407
            std::list<DispatchMsgMapIterator> deregisterList;

            for ( DispatchMsgMapIterator miter( iters.first ); miter != miterEnd; ++miter )
            {
                CEvolMsgReceiver *receiver( ( CEvolMsgReceiver* ) miter->second.first );

                // do not send to itself when the Source Object is assigned
                if ( receiver == msg->getSourceObject() )
                	continue;

                receiver->addEvolMessage( msg );

                // deregister the receiver for this message.
                if ( !miter->second.second )
                	deregisterList.push_back( miter );
            }

            // deregister!
            for ( std::list<DispatchMsgMapIterator>::iterator miter( deregisterList.begin() ); miter != deregisterList.end(); ++miter )
            	dispatchMsgMap.erase( *miter );
        }
        catch (...)
        {
	        Debug::trace("CMessageCenter.error", HobString("dispatch():  catch(...)"));
            LogCatchAndThrow;
        }
    }

    DWORD CMessageCenter::Run()
    {
		try
		{
			// wait for new message
			for ( ;; )
				if ( WAIT_OBJECT_0 == Util::MSGWaitForSingleObject( incomingStream.GetNewMessSemaphore() ) )
                {
                    Util::CSmartPtr< EvolMessageWrapper > p_message;

                    incomingStream >> p_message;

                    // in case someone sends a NULL
                    if ( NULL != p_message.get() )
                        dispatch( p_message );
                }
		}
		catch ( Util::CTerminateNow & )
		{
			Debug::trace("CMessageCenter.error", "Run():  catch(Util::CTerminateNow &)");
			return -1;
		}
		catch ( Util::CBaseException &e )
		{
			Debug::trace("CMessageCenter.error", "Run():  catch(Util::CBaseException & e)");
			UTIL_REPORT_EXCEPTION(e);
			return -1;
		}
		catch (...)
		{
			Debug::trace("CMessageCenter.error", "Run():  catch(...)");
			LogCatch;
			return -1;
		}

        return 0;
    }

    CMessageCenter& CMessageCenter::operator << ( EvolMessage * msg )
    {
    	if ( msg )
    	{
			try
			{
				incomingStream <<  Util::CSmartPtr< EvolMessageWrapper > ( new EvolMessageWrapper( msg ) );
			}
			catch (...)
			{
				Debug::trace("CMessageCenter.error", HobString("CMessageCenter::operator << ():  catch(...)"));
				LogCatchAndThrow;
			}
    	}

        return *this;
    }

    void CMessageCenter::dispatchEvent( const EvolMessage * msg, CEvolMsgReceiver * receiver )
    {
        try
        {
            if ( receiver ) //bypass the stream of Message Center and put to receiver's Sam Stream
            {
                Util::CSmartPtr<EvolMessageWrapper> smart_ptr(new EvolMessageWrapper(msg));
                receiver->addEvolMessage(smart_ptr);
            }
            else
            {
                incomingStream << Util::CSmartPtr< EvolMessageWrapper > ( new EvolMessageWrapper( msg ) );
            }
        }
        catch (...)
        {
		    Debug::trace("CMessageCenter.error", HobString("dispatchEvent(1):  catch(...)"));
            LogCatchAndThrow;
        }
    }

    void CMessageCenter::dispatchEvent( const char * msg, int length, CEvolMsgReceiver * receiver )
    {
        try
        {
            if ( receiver ) //bypass the stream of Message Center and put to receiver's Sam Stream
            {
                Util::CSmartPtr<EvolMessageWrapper> smart_ptr(new EvolMessageWrapper(msg, length));
                receiver->addEvolMessage(smart_ptr);
            }
            else
            {
                incomingStream << Util::CSmartPtr< EvolMessageWrapper > ( new EvolMessageWrapper( msg, length ) );
            }
        }
        catch (...)
        {
		    Debug::trace("CMessageCenter.error", HobString("dispatchEvent(2):  catch(...)"));
            LogCatchAndThrow;
        }
    }


    void CMessageCenter::registerEvent( EvolDeviceType evol_device, UINT8 evol_event, CEvolMsgReceiver* receiver, bool permanent )
    {
        try
        {
            const uint32_t msg_id( ( evol_device << 16 ) + evol_event );

            Util::CLockCriticalSection lock( critSec );

            const std::pair< DispatchMsgMapIterator, DispatchMsgMapIterator> iters( dispatchMsgMap.equal_range( msg_id ) );
            const DispatchMsgMapIterator miterEnd( iters.second );

            bool registered( false );

            for ( DispatchMsgMapIterator miter( iters.first ); miter != miterEnd; ++miter )
            {
                const CEvolMsgReceiver *rec( ( CEvolMsgReceiver* ) miter->second.first );

                if ( rec == receiver )
                {
                    registered = true;
                    break;
                }
            }

            if ( !registered )
                dispatchMsgMap.insert( std::make_pair( msg_id, std::make_pair( receiver, permanent ) ) );
        }
        catch (...)
        {
		    Debug::trace("CMessageCenter.error", HobString("registerEvent():  catch(...)"));
            LogCatchAndThrow;
        }
    }

    void CMessageCenter::deregisterEvent( EvolDeviceType evol_device, UINT8 evol_event, CEvolMsgReceiver* receiver )
    {
        try
        {
            const uint32_t msg_id( ( evol_device << 16 ) + evol_event );

            Util::CLockCriticalSection lock( critSec );

            const std::pair< DispatchMsgMapIterator, DispatchMsgMapIterator> iters( dispatchMsgMap.equal_range( msg_id ) );
            const DispatchMsgMapIterator miterEnd( iters.second );

            for ( DispatchMsgMapIterator miter( iters.first ); miter != miterEnd; ++miter )
            {
                CEvolMsgReceiver *rec( ( CEvolMsgReceiver* ) miter->second.first );

                if ( rec == receiver )
                {
                    dispatchMsgMap.erase( miter );
                    break;
                }
            }
        }
        catch (...)
        {
		    Debug::trace("CMessageCenter.error", HobString("deregisterEvent():  catch(...)"));
            LogCatchAndThrow;
        }
    }

    void CMessageCenter::deregisterAllEvents( CEvolMsgReceiver* receiver )
    {
        try
        {
            Util::CLockCriticalSection lock( critSec );

            DispatchMsgMapIterator iter( dispatchMsgMap.begin() );

            while ( iter != dispatchMsgMap.end() )
            {
                if ( iter->second.first == receiver )
                {
                    DispatchMsgMapIterator temp( iter );
                    ++temp;

                    dispatchMsgMap.erase( iter );

                    iter = temp;
                }
                else
                {
                    ++iter;
                }
            }
        }
        catch (...)
        {
		    Debug::trace("CMessageCenter.error", HobString("deregisterAllEvents():  catch(...)"));
            LogCatchAndThrow;
        }
    }

}// end of namespace

