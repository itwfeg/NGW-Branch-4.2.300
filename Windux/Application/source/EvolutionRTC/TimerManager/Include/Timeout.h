/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef TIMEOUT_H
#define TIMEOUT_H

#include "evolution.h"
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"
#include <time.h>

class Timer;

class Timeout : public EvolMessage
{
	public:
		Timeout( Hobart::CEvolMsgReceiver *pReceiver = NULL, const int tId = 0, unsigned int sId = 0, bool sIdSet = false);
		Timeout(const char *buffer);
		~Timeout();

    #if defined(LINUX_FIX)
		int getTimerId() const;
		unsigned int getMySessionId() const;
    #else
		const int getTimerId() const;
		const unsigned int getMySessionId() const;
    #endif
		bool isSessionIdSet() const;

		void operator=(const Timeout &timeout);

		const char *serialize() const;

	private:
		int		timerId;
		unsigned int	mySessionId;
		bool				sessionIdSet;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif //TIMEOUT_H
