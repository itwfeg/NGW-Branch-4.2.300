#ifndef ITIMERHANDLER_H
#define ITIMERHANDLER_H



namespace Hobart
{


	class ITimerHandler
	{
	public:
		ITimerHandler() {}
		virtual ~ITimerHandler() {}


		//Incoming message processing.
		virtual void timeoutProcess( int timer_id ) = 0;
	};


}



#endif /* ITIMERHANDLER_H */
