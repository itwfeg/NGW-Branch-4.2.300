/**
 * @file BoardSiteReqTestMode.h
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for requesting (functional testing ) board site test mode.
 * Created on: Feb 4, 2020
 */
#ifndef BOARDSITEREQTESTMODE_H
#define BOARDSITEREQTESTMODE_H
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class BoardSiteReqTestMode : public EvolMessage
{
	public:
	BoardSiteReqTestMode(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	BoardSiteReqTestMode(const char *buffer);
	~BoardSiteReqTestMode();

	const char *serialize() const;
};
#endif