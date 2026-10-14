/**
 * @file BoardSiteEvent.h
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief
 * Created on: Feb 3, 2020 
 * @{
 */

#ifndef BOARDSITEEVENT_H
#define BOARDSITEEVENT_H

typedef enum
{
    BrdSiteReqTestMode,
    BrdSiteTestStatus,
    BrdSiteBoardTest,
    BrdSiteBoardTestComplete,
    BrdSiteBoardTestCompleteUI,
    BrdSiteTestReqStatus,
    BrdSitePeripheralAttached,
    BrdSiteReqCompleteResults,
    BrdSitePercentCompete,
    BrdSitePeripheralKPCModel,
    BrdSitePeripheralFlashPass,
    BrdSitePeripheralFlashFail,
    BrdSitePeripheralValuePass,
    BrdSitePeripheralCutterStat
}BoardSiteEvent;


#endif