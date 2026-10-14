#include "MsgReceiverProxy.h"


#include "CEvolMsgManager.h"


#include "ProxyMessage.h"
#include "EventDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace Hobart
{


	#ifdef USE_ACCOUNTANT
		int MsgReceiverProxy::_dataType_ = 0;
		std::string MsgReceiverProxy::_dataTypeName_("Hobart::MsgReceiverProxy");
	#endif


	MsgReceiverProxy::MsgReceiverProxy(Hobart::CEvolMsgReceiver* targetMsgReceiver)
		: msgManager_(0), targetMsgReceiver_(targetMsgReceiver), sequenceNumber_(0), requestCounter_(0), stopProcessing_(false)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif


		msgManager_ = new Hobart::CEvolMsgManager(this, "Hobart::MsgReceiverProxy", false, false, true);
	}


	MsgReceiverProxy::~MsgReceiverProxy()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	Util::CSmartPtr<EvolMessageWrapper> MsgReceiverProxy::processAsProxyMessage(Util::CSmartPtr<EvolMessage> message)
	{
		Util::CMSGEvent waitEvent;
		EvolMessage dummyMessage((EvolDeviceType)0, 0);
		Util::CSmartPtr<EvolMessageWrapper> response(new EvolMessageWrapper(&dummyMessage));


		incrementRequestCounter();
		if ( !stopProcessing_ )
		{
			/* Create a proxy message for the original message and dispatch it to this Proxy. */
			Util::CSmartPtr<Hobart::ProxyMessage> proxyMessage(new Hobart::ProxyMessage(message.get(), waitEvent, response.get()));
//			msgManager_->dispatchEvent(proxyMessage, msgManager_);
			msgManager_->send(proxyMessage);


			/* Wait on the event, which is fired after receiving the response. */
			waitEvent.Wait();
		}
		decrementRequestCounter();

		/* Return the response. */
		return response;
	}


	void MsgReceiverProxy::processEvent(Util::CSmartPtr<EvolMessageWrapper>& msg)
	{
		if ( msg.notNull() )
		{
			if ( msg->getDeviceType() == Event && msg->getEventType() == ProxyEvent )
			{
				int seqNumber = getSequenceNumber();
				Util::CSmartPtr<ProxyMessage> proxyMessage(new ProxyMessage(msg->Data()));


				if ( !stopProcessing_ )
				{
					addProxyContext(seqNumber, new ProxyContext(proxyMessage->getEvent(), proxyMessage->getResponse()));


					/* Create an EvolMessageWrapper out of the original message in order
					to be able to modify it so that the response comes to this Proxy. */
					const std::string& originalMessage = proxyMessage->getOriginalMessage();
					EvolMessageWrapper wrapper(originalMessage.data(), originalMessage.size());
					wrapper.setSourceObject(msgManager_);
					wrapper.setSequenceNumber(seqNumber);


					/* Dispatch modified original message to target MsgReceiver. */
					msgManager_->dispatchEvent(wrapper.Data(), wrapper.Length(), targetMsgReceiver_);
				}
				else
				{
					/* If we don't touch the response then they'll "receive" the original dummyMessage
					as response to their request. */
					Util::CMSGEvent event_ = proxyMessage->getEvent();
					event_.EventSet(); 
				}
			}
			else
			{
				Util::CSmartPtr<ProxyContext> proxyContext(extractProxyContext(msg->getSequenceNumber()));
				if ( proxyContext.notNull() )
				{
					*proxyContext->getResponse() = *msg.get();
					proxyContext->signalEvent();
				}
			}
		}
	}


	void MsgReceiverProxy::addProxyContext(int sequenceNumber, ProxyContext* proxyContext)
	{
		util_thread::Lock< util_thread::Mutex<> > lock(containerMutex_);

		requests_.insert(std::make_pair(sequenceNumber, proxyContext));
	}


	MsgReceiverProxy::ProxyContext* MsgReceiverProxy::extractProxyContext(int sequenceNumber)
	{
		ProxyContext* context = 0;
		util_thread::Lock< util_thread::Mutex<> > lock(containerMutex_);

		requestsIterator_type iter = requests_.find(sequenceNumber);
		if ( iter != requests_.end() )
		{
			context = iter->second;

			requests_.erase(iter);
		}

		return context;
	}


	MsgReceiverProxy::ProxyContext* MsgReceiverProxy::extractFirstProxyContext()
	{
		ProxyContext* context = 0;
		util_thread::Lock< util_thread::Mutex<> > lock(containerMutex_);

		requestsIterator_type iter = requests_.begin();
		if ( iter != requests_.end() )
		{
			context = iter->second;

			requests_.erase(iter);
		}

		return context;
	}


	bool MsgReceiverProxy::waitUntilIdle()
        {
            if ( stopProcessing_ )
            {
                do
                {
                    ProxyContext* context = extractFirstProxyContext();
                    while ( context )
                    {
                        /* If we don't touch the response then they'll "receive" the original dummyMessage
                           as response to their request. */
                        context->signalEvent();
                        context = extractFirstProxyContext();
                    }
#ifdef LINUX_FIX
                    PAL_absclass::PAL_sleep( 50 );
#else
                    Sleep(50);
#endif
                } while ( getRequestCounter() );
                return true;
            }
            return false;
        }
}
