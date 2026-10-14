#include "PAL_platformheader.h"



#include "SimpleTimer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

namespace Hobart
{

#ifdef USE_ACCOUNTANT
	int SimpleTimer::dataType = 0;
	std::string SimpleTimer::dataTypeName("SimpleTimer");
#endif


	SimpleTimer::SimpleTimer(int id, int due_time, ITimerHandler * handler, bool periodic)
		: timerId_(id), milliseconds_(due_time), periodic_(periodic), started_(true), done_(false), handler_(handler)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		char threadName[256];
		sprintf(threadName, "Timer Thread %d", timerId_);

		timerThread_ = new Util::CTLSThread(*this, THREAD_PRIORITY_NORMAL, 0, threadName);
	}


	SimpleTimer::SimpleTimer(int dueTime, bool periodic, bool start)
		: timerId_(-1), milliseconds_(dueTime), periodic_(periodic), started_(start), done_(false), handler_(0)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif
		timerThread_ = new Util::CTLSThread(*this, THREAD_PRIORITY_NORMAL, 0, "Timer Thread", started_);
	}

	SimpleTimer::~SimpleTimer()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
		//shutdown the timer thread
		if (timerThread_)
		{
			timerThread_->StopThread();
			timerThread_->WaitForCompletion();

			// this will delete thread
			timerThread_ = NULL;
		}
	}

	bool SimpleTimer::isTimeoutThread() const
	{
		return (threadId_ == std::this_thread::get_id());
	}

	void SimpleTimer::start()
	{
		if (!started_)
		{
			started_ = true;
			timerThread_->RestartThread();
		}
	}

	void SimpleTimer::stop()
	{
		if ((started_) && (!done_))
		{
			// when current thread is timeout thread signal for thread to complete, otherwise stop thread
			if ( isTimeoutThread() )
				m_ExitEvent.EventSet();
			else if (timerThread_)
				timerThread_->StopThread();
		}
	}

	DWORD SimpleTimer::Run()
	{
		threadId_ = std::this_thread::get_id();

		try
		{
			do
			{
				if (PAL_absclass::PAL_waitForSingleObj(m_ExitEvent, milliseconds_) == WAIT_TIMEOUT)
				{
					if (timerId_ == -1)
						handleTimeout();
					else
						handler_->timeoutProcess(timerId_);
				}
			} 
			while (!m_ExitEvent.IsEventSet() && periodic_);

			done_ = true;

			return 0;
		}
		catch (Util::CTerminateNow)
		{
			return 1;
		}
		catch (...)
		{
			LogCatch;
			return -1;
		}
	}


}
