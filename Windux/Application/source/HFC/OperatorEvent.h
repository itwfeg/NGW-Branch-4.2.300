#ifndef OPERATOREVENT_H
#define OPERATOREVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	// Operator Generated Events - OperatorDev
	typedef enum OperatorEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		OpLogin = 20,
		OpLogout,
		OpLoginRFID,
		OpLogoutRFID,
		OpNotifyUIofRFIDInputStart,
		OpNotifyUIofRFIDInputEnd,
		OpDoesRFIDExist,
		OpRFIDExists,
		OpRFIDDoesntExist,
		OpPromptForIDOrRFID,

		OpRenumberOperator = 30,
		OpRemoteLoginRequired,
		OpRemoteLogin,
		
		OpPromptForIDOrRFIDForFieldAccess = 39,

	    OpDoesIDExist = 46,
	    OpIDExists,
	    OpIDDoesntExist,

		OpReadNote				= 50,
		OpReadNoteForEditing,
		OpWriteNote,
		OpDeleteNote			= 54,
		OpSaveNote				= 56,
		OpRestoreNote			= 57
	}   OperatorEvent_t;

	typedef enum OperatorDevError
	{
		OperatorErrUnknownError = OperatorDev * DEVICE_ERROR_OFFSET,
		OperatorErrLoginNotFound,
		OperatorErrInvalidServiceLoginId,
		OperatorErrRecordNotFound,
		OperatorChangeErrNoRecordsFound,
		OperatorChangeErrRecordNotFound,		//	102005
		OperatorErrAccessNotAllowed,
		OperatorErrOperatorNote,
		OperatorErrCannotDeleteNonExistingOp,
		OperatorErrEnterIDorRFID,
		OperatorErrRenumberWarning,				//	102010
		OperatorErrRenumberOperator,
		OperatorErrOpNumAlreadyExists,
		OperatorErrOpRFIDAlreadyExists,
		OperatorErrRFIDOperatorNotFound,
		OperatorErrNotLoggedIn,					//	102015		HTA-38
		OperatorErrAccessDenied
	}   OperatorDevError_t;

#endif // OPERATOREVENT_H
