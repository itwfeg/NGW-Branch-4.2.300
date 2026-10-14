#ifndef GRAPHICEVENT_H
#define GRAPHICEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	// Graphic Generated Events - GraphicDev
	typedef enum GraphicEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		GraphicReadAllNames = 20,

		ReadRomGraphic = 21,
		ReadAllRomGraphics,
		WriteRomGraphic,
		WriteAllRomGraphics,
		SaveRomGraphic,
		RestoreRomGraphic,
		ReadAllRomGraphicNames,

		ReadProductGraphic = 40,
		ReadAllProductGraphics,
		WriteProductGraphic,
		WriteAllProductGraphics,
		DeleteProductGraphic,
		DeleteAllProductGraphics,
		SaveProductGraphic,
		RestoreProductGraphic,
		ReadAllProductGraphicNames = 55,
		BatchApplyDeleteProductGraphic,

		ReadCategoryGraphic = 60,
		ReadAllCategoryGraphics,
		WriteCategoryGraphic,
		WriteAllCategoryGraphics,
		DeleteCategoryGraphic,
		DeleteAllCategoryGraphics,
		SaveCategoryGraphic,
		RestoreCategoryGraphic,
		ReadAllCategoryGraphicNames = 75,
		BatchApplyDeleteCategoryGraphic,

		ReadSpecialsGraphic = 80,
		ReadAllSpecialsGraphics,
		WriteSpecialsGraphic,
		WriteAllSpecialsGraphics,
		DeleteSpecialsGraphic,
		DeleteAllSpecialsGraphics,
		SaveSpecialsGraphic,
		RestoreSpecialsGraphic,
		ReadAllSpecialsGraphicNames = 95,
		BatchApplyDeleteSpecialsGraphic,

		ReadStreamingGraphic = 100,
		ReadAllStreamingGraphics,
		WriteStreamingGraphic,
		WriteAllStreamingGraphics,
		DeleteStreamingGraphic,
		DeleteAllStreamingGraphics,
		SaveStreamingGraphic,
		RestoreStreamingGraphic,
		ReadAllStreamingGraphicNames = 115,
		BatchApplyDeleteStreamingGraphic,

		ReadSelfServeGraphic = 120,
		ReadAllSelfServeGraphics,
		WriteSelfServeGraphic,
		WriteAllSelfServeGraphics,
		DeleteSelfServeGraphic,
		DeleteAllSelfServeGraphics,
		SaveSelfServeGraphic,
		RestoreSelfServeGraphic,
		ReadAllSelfServeGraphicNames = 135,
		BatchApplyDeleteSelfServeGraphic,

		ReadSoftkeyGraphic = 140,
		ReadAllSoftkeyGraphics,
		WriteSoftkeyGraphic,
		WriteAllSoftkeyGraphics,
		DeleteSoftkeyGraphic,
		DeleteAllSoftkeyGraphics,
		SaveSoftkeyGraphic,
		RestoreSoftkeyGraphic,
		ReadAllSoftkeyGraphicNames = 155,
		BatchApplyDeleteSoftkeyGraphic,

		ReadPlanogramGraphic = 160,
		ReadAllPlanogramGraphics,
		WritePlanogramGraphic,
		WriteAllPlanogramGraphics,
		DeletePlanogramGraphic,
		DeleteAllPlanogramGraphics,
		SavePlanogramGraphic,
		RestorePlanogramGraphic,
		ReadAllPlanogramGraphicNames = 175,
		BatchApplyDeletePlanogramGraphic,
		NewPlanogramReceived,
		ReadMostRecentPlanogramGraphic,
		WriteMostRecentPlanogramGraphic,
		AcknowledgeMostRecentPlanogramGraphic,
		CheckForNewPlanogramReceived,

		ReadVideo = 180,								// reserved, not implemented
		ReadAllVideos,									// reserved, not implemented
		WriteVideo,										// reserved, not implemented
		WriteAllVideos,									// reserved, not implemented
		DeleteVideo,									// reserved, not implemented
		DeleteAllVideos,								// reserved, not implemented
		SaveVideo,										// reserved, not implemented
		RestoreVideo,									// reserved, not implemented
		ReadAllVideoNames = 195,
		BatchApplyDeleteVideo							// reserved, not implemented

	}   GraphicEvent_t;

	typedef enum GraphicDevError
	{
		GraphicErrUnknownError = GraphicDev * DEVICE_ERROR_OFFSET,
		GraphicErr
	}   GraphicDevError_t;

#endif // GRAPHICEVENT_H
