#ifndef SIMPLETIMER_H
#define SIMPLETIMER_H



#include "ITimerHandler.h"


#include "CTLSThread.h"
#include "CSmartPtr.h"

#include <thread>


namespace Hobart
{


	class SimpleTimer : public Util::CRunnableClass
	{
	public:
		/* --- Constructors and destructors --- */
		SimpleTimer( int id , int due_time, ITimerHandler * handler, bool periodic = false );
		SimpleTimer( int dueTime, bool periodic = false, bool start = false );
		virtual ~SimpleTimer();


		/* --- Getters and setters --- */
		int getId() const { return timerId_; };

		bool isDone() const { return done_; };
		
		bool isTimeoutThread() const;

		/* --- Instance function members --- */
		void start();
		void stop();

		virtual void handleTimeout() {}

		void stopPeriodicTimer() {periodic_ = false;}

		void shutdownThread() { if (timerThread_) { timerThread_->StopThread(); timerThread_->WaitForCompletion(); } }
	
	private:
		/* --- Instance data members --- */
		Util::CSmartPtr<Util::CTLSThread> timerThread_;
		int timerId_;
		int milliseconds_;
		bool periodic_;
		bool started_;
		bool done_;
		ITimerHandler * handler_;

		std::thread::id threadId_;


		/* --- Instance function members --- */
		DWORD Run();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
	};


}//end of namespace



#endif /* SIMPLETIMER_H */
