#ifndef SOFTKEYDEVEVENT_H
#define SOFTKEYDEVEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"


	// Softkey Generated Events
	typedef enum SoftkeyDevEvent
	{
		//	Softkey
		ReadSoftkey						= 20,
		ReadAllSoftkeys,
		WriteSoftkey,
		WriteAllSoftkeys,
		DeleteSoftkey,
		DeleteAllSoftkeys,
		SaveSoftkey,					// 26
		RestoreSoftkey,
		FirstSoftkey,					// 28
		LastSoftkey,
		NextSoftkey,					// 30
		PreviousSoftkey,
		CreateNewSoftkey,				// 32
		RestoreFlashkeyAsSoftkey,
		ExecuteSoftkey,					// 34
		BatchApplyDeleteSoftkey,
		ReadSoftkeyScripts,
		ListSoftkeyScripts,
		SoftkeyScriptResult,

		//	SoftkeyFeature
		ReadSoftkeyFeature				= 40,
		ReadAllSoftkeyFeatures,
		WriteSoftkeyFeature,
		WriteAllSoftkeyFeatures,
		DeleteSoftkeyFeature,
		DeleteAllSoftkeyFeatures,
		SaveSoftkeyFeature,				// 46
		RestoreSoftkeyFeature,
		FirstSoftkeyFeature,			// 48
		LastSoftkeyFeature,
		NextSoftkeyFeature,				// 50
		PreviousSoftkeyFeature,
		CreateNewSoftkeyFeature,		// 52
		BatchApplyDeleteSoftkeyFeature,

		//	SoftkeyStyle
		ReadSoftkeyStyle				= 60,
		ReadAllSoftkeyStyles,
		WriteSoftkeyStyle,
		WriteAllSoftkeyStyles,
		DeleteSoftkeyStyle,
		DeleteAllSoftkeyStyles,
		SaveSoftkeyStyle,				// 66
		RestoreSoftkeyStyle,
		FirstSoftkeyStyle,				// 68
		LastSoftkeyStyle,
		NextSoftkeyStyle,				// 70
		PreviousSoftkeyStyle,
		CreateNewSoftkeyStyle,			// 72
		ReadAllSoftkeyStyleNumbers,
		WriteAllSoftkeyStyleNumbers,	// 74
		BatchApplyDeleteSoftkeyStyle,

		//	SoftkeyGroup
		ReadSoftkeyGroup				= 80,
		ReadAllSoftkeyGroups,
		WriteSoftkeyGroup,
		WriteAllSoftkeyGroups,
		DeleteSoftkeyGroup,
		DeleteAllSoftkeyGroups,
		SaveSoftkeyGroup,				// 86
		RestoreSoftkeyGroup,
		FirstSoftkeyGroup,				// 88
		LastSoftkeyGroup,
		NextSoftkeyGroup,				// 90
		PreviousSoftkeyGroup,
		CreateNewSoftkeyGroup,			// 92
		LoadSoftkeyGroup,						//	Used to display the entire softkey group (was previously Read/Write SoftkeyGroup)
		BatchApplyDeleteSoftkeyGroup,
		LoadSoftkeyGroupForEditor,

		ListCameraSystemPredictions		= 120,
		ReadCameraSystemPredictions,

		LoadAllSoftkeyGroups = 255
	}   SoftkeyDevEvent_t;

	typedef enum SoftkeyDevError
	{
		SoftkeyErrUnknownError = SoftkeyDev * DEVICE_ERROR_OFFSET,
		SoftkeyNotFound,

		SoftkeyFeatureErrUnknownError = SoftkeyErrUnknownError + 200,
		SoftkeyFeatureNotFound,

		SoftkeyStyleErrUnknownError = SoftkeyFeatureErrUnknownError + 200,
		SoftkeyStyleNotFound,

		SoftkeyGroupErrUnknownError = SoftkeyStyleErrUnknownError + 200,
		SoftkeyGroupNotFound,
	}   SoftkeyDevError_t;

#endif // SOFTKEYDEVEVENT_H
