#ifndef IEVOLMSGHANDLER_H
#define IEVOLMSGHANDLER_H



#include "EvolMessageWrapper.h"
#include "CSmartPtr.h"



namespace Hobart
{


	class IEvolMsgHandler
	{
	public:
		IEvolMsgHandler() {}
		virtual ~IEvolMsgHandler() {}


		//Incoming message processing.
		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg ) = 0;
	};


}



#endif /* IEVOLMSGHANDLER_H */
