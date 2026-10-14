#ifndef BOARDSITEGENERIC_H
#define BOARDSITEGENERIC_H
#include "BoardSiteEvent.h"
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class BoardSiteGeneric : public EvolMessage
{
	public:
		BoardSiteGeneric( BoardSiteEvent bsEvent, Hobart::CEvolMsgReceiver *pHobThread = NULL );
		BoardSiteGeneric( const char *buffer );
		virtual ~BoardSiteGeneric();


	private:
        BoardSiteGeneric();
};



#endif