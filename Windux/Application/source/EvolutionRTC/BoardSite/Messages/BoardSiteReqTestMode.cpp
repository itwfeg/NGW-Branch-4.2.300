/**
 * @file BoardSiteReqTestMode.cpp
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for requesting (functional testing ) board site test mode.
 * Created on: Feb 4, 2020
 */
#include "BoardSiteReqTestMode.h"
#include "BoardSiteEvent.h"

BoardSiteReqTestMode::BoardSiteReqTestMode(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(BoardSite, BrdSiteReqTestMode, pHobThread)
{}

BoardSiteReqTestMode::BoardSiteReqTestMode(const char *buffer) : EvolMessage(buffer)
{}

BoardSiteReqTestMode::~BoardSiteReqTestMode()
{}

const char *BoardSiteReqTestMode::serialize() const
{
    EvolMessage::serialize();
    return serializedData.data();
}
