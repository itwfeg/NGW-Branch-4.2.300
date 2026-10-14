#ifndef CGICALLMANAGER_H
#define CGICALLMANAGER_H



#include "IEvolMsgHandler.h"

#include "CEvolMsgManager.h"
#include "CGICallerI.h"


#include "util/thread/mutex.h"



namespace comm
{


	class CGICallManager : public Hobart::IEvolMsgHandler
	{
		public:
			/* --- Constructors and desctructor --- */
			// The constructor is private to properly implement the Singleton pattern
			static CGICallManager *getInstance(CGICallerI* caller);
			~CGICallManager();


			/* --- Getter and setter methods --- */
			Hobart::CEvolMsgManager* getMessageReceiver() { return msgManager_.get(); }


			/* --- Instance Function Members --- */
			void processEvent(Util::CSmartPtr<EvolMessageWrapper> & msg);


		private:
			/* --- Instance Data Members --- */
			Util::CSmartPtr<Hobart::CEvolMsgManager> msgManager_;
			Util::CSmartPtr<CGICallerI> cgiCaller_;


			/* --- Constructors --- */
			CGICallManager(CGICallerI* caller);
			// Do not allow the default copy constructor.
			CGICallManager(const CGICallManager&);


			/* --- Instance Function Members --- */
			// Do not allow the default assignment operator.
			CGICallManager operator=(const CGICallManager& rhs);


			/* --- Class Data Members --- */
			// Singleton data members
			static CGICallManager* _instance_;
			static util_thread::Mutex<> _singletonMutex_;

		#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
		#endif
	};


}



#endif //CGICALLMANAGER_H
