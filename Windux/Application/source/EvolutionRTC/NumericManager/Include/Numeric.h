
#ifndef NUMERIC_H
#define NUMERIC_H

#ifdef __cplusplus
	#pragma pack(1)
#endif

	#define MAX_NUMERIC_STRING      40
	#define MAX_DEVICE_NAME_STRING      6

	// Different Kinds of Numerics
	typedef enum NumericKind
	{
		NmNoNumeric,
		NmUsbEvol,
		NmUsbApc
	}   NumericKind_t;

	/* Numeric Configuration Status */
	typedef enum NmCfgStatus
	{
		NumericConfigValid,
		NumericConfigError
	}   NmCfgStatus_t;

	/* Display Type */
	typedef enum NmDisplayType
	{
		NumericInvalidDisplay,
		NumericVacuumFlourescentDisplay,
		NumericGasPlasmaDisplay
	}   NmDisplayType_t;

	typedef enum NmConfigType
	{
		NmPermanent = 0x0000,
		NmTemporary = 0x0001,
		NmDefault = 0x0002
	}   NmConfigType_t;

	typedef enum NmInquireDisp
	{
		NmValid = 0x0000,
		NmError = 0x0001
	}   NmInquireDisp_t;

	typedef enum NmModeVal
	{
		Undefined1,
		Undefined2,
		Undefined3 //here to force size to two bytes
	}   NmModeVal_t;

	typedef enum NmTestVal
	{
		NmAllOff,
		NmAllOn,
		NmCheckerBoard,
		NmCharacter,
		NmAnnunciator
	}   NmTestVal_t;

	typedef enum NmFieldType
	{
		NmTare,
		NmWeight,
		NmUnitPrice,
		NmTotalPrice,
		NmNumeric,
		NmPoundsPerPrice,
		NmInvalid
	}   NmFieldType_t;

	typedef enum NmAnnuncicatorIndex
	{
		NmCode0,
		NmCode1,
		NmCode2,
		NmCode3,
		NmCode4,
		NmCode5,
		NmCode6,
		NmCode7,
		NmCode8,
		NmCode9,
		NmCode10,
		NmCode11,
		NmCode12,
		NmCode13,
		NmCode14,
		NmCode15,
		NmCode16,
		NmCode17,
		NmCode18,
		NmCode19,
		NmCodeInvalid
	}   NmAnnuncicatorIndex_t;

	typedef enum NmState
	{
		NmOff,
		NmOn
	}   NmState_t;

	typedef enum NmOperationMode
	{
		NmOpModeNormal,
		NmOpModeOutOfServ

	}   NmOperationMode_t;

#ifdef __cplusplus
	#pragma pack()
#endif

#endif //NUMERIC_H
