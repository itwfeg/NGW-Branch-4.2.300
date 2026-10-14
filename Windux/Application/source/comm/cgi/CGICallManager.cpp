#include "CGICallManager.h"
#include "CGICallManagerEvents.h"
#include "CGICallerI.h"
#include "PerformCGICallMessage.h"


#include "Configuration.h"

#include "util/e_string/utilstring.h"
#include "util/thread/lock.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace comm
{


#ifdef USE_ACCOUNTANT
    int CGICallManager::_dataType_ = 0;
    std::string CGICallManager::_dataTypeName_("comm::CGICallManager"); 
#endif


    CGICallManager* CGICallManager::_instance_ = 0;
    util_thread::Mutex<> CGICallManager::_singletonMutex_;


    CGICallManager::CGICallManager(CGICallerI* caller)
        : msgManager_(0), cgiCaller_(caller)
    {
#ifdef USE_ACCOUNTANT
        if ( _dataType_ == 0 )
            _dataType_ = util_acc::registerDataType(_dataTypeName_);

        util_acc::addInstance(_dataType_);
#endif

        Debug::trace("CGICallManager", "Initializing CGICallManager");

        msgManager_ = new Hobart::CEvolMsgManager(this, "CGICallManager", false, false, false);		
        msgManager_->RestartThread();
        msgManager_->registerEvent(CGICallManagerDev, PerformCGICall);

        Debug::trace("CGICallManager", "Registered for PerformCGICall events");
    }


    CGICallManager::~CGICallManager()
    {
#ifdef USE_ACCOUNTANT
        util_acc::removeInstance(_dataType_);
#endif
    }


    CGICallManager *CGICallManager::getInstance(CGICallerI* caller)
    {
        if ( _instance_ == 0 )
        {
            util_thread::Lock< util_thread::Mutex<> > lock(_singletonMutex_);

            if ( _instance_ == 0 )
                _instance_ = new CGICallManager(caller);
        }

        return _instance_;
    }


    void CGICallManager::processEvent( Util::CSmartPtr<EvolMessageWrapper> & msg )
    {
        if ( msg.notNull() )
        {
            //			unsigned threadId = CEvolMsgReceiver::GetThreadID();

            //			SessionManager::getInstance()->requestToContinue(threadId, false);
            bool isTracing = Debug::isTracing("CGICallManager");

            switch ( msg->getDeviceType() )
            {
                case CGICallManagerDev:
                    {
                        switch ( msg->getEventType())
                        {
                            case PerformCGICall:
                                {
                                    if ( isTracing )
                                        Debug::trace("CGICallManager", "Received PerformCGICall event");

                                    if ( msg.notNull() )
                                    {
                                        Util::CSmartPtr<PerformCGICallMessage> cgicall(new PerformCGICallMessage(msg->Data()));

                                        if ( isTracing )
                                            Debug::trace("CGICallManager", "Requesting CGI call for url: " + cgicall->getURL());

										if ( cgiCaller_ && cgicall )
											cgiCaller_->performCGICall(cgicall->getURL(), cgicall->getPayload());
                                    }

                                    break;
                                }
                        }

                        break;
                    }
#ifdef LINUX_FIX
                default:
                    break;
#endif
            }
            //			SessionManager::getInstance()->changeBlockabilityStatus(threadId, true, false);
        }
    }
}
