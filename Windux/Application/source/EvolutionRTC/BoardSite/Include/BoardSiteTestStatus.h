/**
 * @file BoardSiteTestStatus.h
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for reporting board site test status.
 * Created on: Feb 4, 2020
 */
#ifndef BOARDSITETESTSTATUS_H
#define BOARDSITETESTSTATUS_H
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class BoardSiteTestStatus : public EvolMessage
{
	public:
	BoardSiteTestStatus(Hobart::CEvolMsgReceiver *pHobThread = NULL, bool ready = false);
	BoardSiteTestStatus(const char *buffer);
	~BoardSiteTestStatus();

	const char *serialize() const;
    
    bool isReady() const { return ready_; }
	void setStatus( bool ready ) { ready_ = ready; }

    private:
    bool ready_;
};






#endif