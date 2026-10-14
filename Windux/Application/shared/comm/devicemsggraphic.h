/**
 * @file devicemsggraphic,h
 * Graphic Device events and message classes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup communicationsCsu
 * @{
 */

#ifndef DEVICEMSGGRAPHIC_H
#define DEVICEMSGGRAPHIC_H

#include "devicemsg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Graphic Device Event Types.
/**
 * Enumeration of events for the @c deviceMsgType::Graphic device.
 */
class graphicDeviceMsgEventType
{
public:

    /// Event Types.
    enum type
    {
        GraphicReadAllNames                 = 20,

        ReadRomGraphic                      = 21,
        ReadAllRomGraphics,
        WriteRomGraphic,
        WriteAllRomGraphics,
        SaveRomGraphic,
        RestoreRomGraphic,
        ReadAllRomGraphicNames,

        ReadProductGraphic                  = 40,
        ReadAllProductGraphics,
        WriteProductGraphic,
        WriteAllProductGraphics,
        DeleteProductGraphic,
        DeleteAllProductGraphics,
        SaveProductGraphic,
        RestoreProductGraphic,
        ReadAllProductGraphicNames          = 55,
        BatchApplyDeleteProductGraphic,

        ReadCategoryGraphic                 = 60,
        ReadAllCategoryGraphics,
        WriteCategoryGraphic,
        WriteAllCategoryGraphics,
        DeleteCategoryGraphic,
        DeleteAllCategoryGraphics,
        SaveCategoryGraphic,
        RestoreCategoryGraphic,
        ReadAllCategoryGraphicNames         = 75,
        BatchApplyDeleteCategoryGraphic,

        ReadSpecialsGraphic                 = 80,
        ReadAllSpecialsGraphics,
        WriteSpecialsGraphic,
        WriteAllSpecialsGraphics,
        DeleteSpecialsGraphic,
        DeleteAllSpecialsGraphics,
        SaveSpecialsGraphic,
        RestoreSpecialsGraphic,
        ReadAllSpecialsGraphicNames         = 95,
        BatchApplyDeleteSpecialsGraphic,

        ReadStreamingGraphic                = 100,
        ReadAllStreamingGraphics,
        WriteStreamingGraphic,
        WriteAllStreamingGraphics,
        DeleteStreamingGraphic,
        DeleteAllStreamingGraphics,
        SaveStreamingGraphic,
        RestoreStreamingGraphic,
        ReadAllStreamingGraphicNames        = 115,
        BatchApplyDeleteStreamingGraphic,

        ReadSelfServeGraphic                = 120,
        ReadAllSelfServeGraphics,
        WriteSelfServeGraphic,
        WriteAllSelfServeGraphics,
        DeleteSelfServeGraphic,
        DeleteAllSelfServeGraphics,
        SaveSelfServeGraphic,
        RestoreSelfServeGraphic,
        ReadAllSelfServeGraphicNames        = 135,
        BatchApplyDeleteSelfServeGraphic,

        ReadSoftkeyGraphic                  = 140,
        ReadAllSoftkeyGraphics,
        WriteSoftkeyGraphic,
        WriteAllSoftkeyGraphics,
        DeleteSoftkeyGraphic,
        DeleteAllSoftkeyGraphics,
        SaveSoftkeyGraphic,
        RestoreSoftkeyGraphic,
        ReadAllSoftkeyGraphicNames          = 155,
        BatchApplyDeleteSoftkeyGraphic,

        ReadPlanogramGraphic                = 160,
        ReadAllPlanogramGraphics,
        WritePlanogramGraphic,
        WriteAllPlanogramGraphics,
        DeletePlanogramGraphic,
        DeleteAllPlanogramGraphics,
        SavePlanogramGraphic,
        RestorePlanogramGraphic,
        ReadAllPlanogramGraphicNames        = 175,
        BatchApplyDeletePlanogramGraphic,
        NewPlanogramReceived,
        ReadMostRecentPlanogramGraphic,
        WriteMostRecentPlanogramGraphic,
        AcknowledgeMostRecentPlanogramGraphic,
        CheckForNewPlanogramReceived,

        ReadVideo                           = 180,		// reserved, not implemented
        ReadAllVideos,									// reserved, not implemented
        WriteVideo,										// reserved, not implemented
        WriteAllVideos,									// reserved, not implemented
        DeleteVideo,									// reserved, not implemented
        DeleteAllVideos,								// reserved, not implemented
        SaveVideo,										// reserved, not implemented
        RestoreVideo,									// reserved, not implemented
        ReadAllVideoNames                   = 195,
        BatchApplyDeleteVideo							// reserved, not implemented
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef deviceMsgBiDirectional<deviceMsgType::Graphic, graphicDeviceMsgEventType::ReadAllSoftkeyGraphicNames> graphicReadAllSoftkeyGraphicNamesDeviceMsg;
typedef deviceMsgBiDirectional<deviceMsgType::Graphic, graphicDeviceMsgEventType::ReadAllVideoNames> graphicReadAllVideoNamesDeviceMsg;

/** @} */

#endif // DEVICEMSGGRAPHIC_H
