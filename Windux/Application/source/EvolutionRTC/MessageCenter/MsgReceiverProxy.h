#ifndef MSGRECEIVERPROXY_H
#define MSGRECEIVERPROXY_H



#include <map>


#include "IEvolMsgHandler.h"
class EvolMessageWrapper;


#include "MSGEvent.h"
#include "util/thread/mutex.h"
#include "util/thread/lock.h"



namespace Hobart
{


	class CEvolMsgManager;


	class MsgReceiverProxy : public Hobart::IEvolMsgHandler
	{
		public:
			/* --- Types and subtypes --- */
			class ProxyContext
			{
				public:
					ProxyContext(const Util::CMSGEvent& event, EvolMessageWrapper* response)
							: event_(event), response_(response) {}  // this creates another copy of the event handle
					~ProxyContext() {}

					const Util::CMSGEvent& getEvent() const { return event_; }

					EvolMessageWrapper* getResponse() const { return response_; }

					bool signalEvent() { return event_.EventSet() != 0; }


				private:
					Util::CMSGEvent event_;
					EvolMessageWrapper* response_;
			};
			typedef std::map<unsigned int, ProxyContext*> requestsContainer_type;
			typedef requestsContainer_type::iterator requestsIterator_type;


			/* --- Constructors and destructor --- */
			MsgReceiverProxy(Hobart::CEvolMsgReceiver* targetMsgReceiver);
			virtual ~MsgReceiverProxy();


			/* --- Getters and setters --- */
#ifdef LINUX_FIX
			unsigned int getSequenceNumber() 
			{ 
				return PAL_absclass::PAL_InterlockedIncrement((LONG *) &sequenceNumber_);
			} 
#else
			unsigned int getSequenceNumber() { return InterlockedIncrement((LONG*)&sequenceNumber_); }
#endif
			bool isStopProcessing() const { return stopProcessing_; }
			void setStopProcessing() { stopProcessing_ = true; }


			/* --- Instance Function members --- */
			Util::CSmartPtr<EvolMessageWrapper> processAsProxyMessage(Util::CSmartPtr<EvolMessage> message);
			void processEvent(Util::CSmartPtr<EvolMessageWrapper>& msg);
			bool waitUntilIdle();


		protected:
			/* --- Instance data members --- */
			Util::CSmartPtr<Hobart::CEvolMsgManager> msgManager_;
			Hobart::CEvolMsgReceiver* targetMsgReceiver_;
			unsigned int sequenceNumber_;


		private:
			/* --- Instance data members --- */
			requestsContainer_type requests_;
			util_thread::Mutex<> containerMutex_;
			int requestCounter_;
			mutable util_thread::Mutex<> requestCounterMutex_;
			bool stopProcessing_;


			/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif


			/* --- Instance function members --- */
			void incrementRequestCounter() { util_thread::Lock< util_thread::Mutex<> > lock(requestCounterMutex_); ++requestCounter_; }
			void decrementRequestCounter() { util_thread::Lock< util_thread::Mutex<> > lock(requestCounterMutex_); --requestCounter_; }
			int getRequestCounter() const  { util_thread::Lock< util_thread::Mutex<> > lock(requestCounterMutex_); return requestCounter_; }

			void addProxyContext(int sequenceNumber, ProxyContext* proxyContext);
			ProxyContext* extractProxyContext(int sequenceNumber);
			ProxyContext* extractFirstProxyContext();
	};


}



#endif /* MSGRECEIVERPROXY_H */
