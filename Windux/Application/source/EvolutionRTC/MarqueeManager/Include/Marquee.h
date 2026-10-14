
#ifndef MARQUEE_H
#define MARQUEE_H

	#define MAX_MARQUEE_STRING      120
	#define MAX_DEVICE_NAME_STRING      6

#ifdef __cplusplus
	#pragma pack(1)
#endif

	// Different Kinds of Numerics
	typedef enum MarqueeKind
	{
		MqNoMarquee,
		MqUsbEvol,
		MqUsbApc
	}   MarqueeKind_t;

	/* Marquee Configuration Status */
	typedef enum MqCfgStatus
	{
		MarqueeConfigValid,
		MarqueeConfigError
	}   MqCfgStatus_t;

	/* Display Type */
	typedef enum MqDisplayType
	{
		MarqueeInvalidDisplay,
		MarqueeVacuumFlourescentDisplay,
		MarqueeGasPlasmaDisplay
	}   MqDisplayType_t;

	typedef enum MqConfigType
	{
		MqPerm = 0x0000,
		MqTemporary = 0x0001,
		MqDefault = 0x0002
	}   MqConfigType_t;

	typedef enum MqInquireDisp
	{
		MqValid = 0x0000,
		MqError = 0x0001
	}   MqInquireDisp_t;

	typedef enum MqProdMode
	{
		SuspendProductMode,
		ProductMode1,
		ProductMode2,
		ProductMode3,
		ProductMode4,
		ProductMode5,
		ProductMode6,
		ProductMode7,
		ProductMode8,
		ProductMode9,
		ProductMode10,
		ProductMode11,
		ProductMode12,
		ProductMode13,
		ProductMode14,
		OnlyProductMode

	}   MqProdMode_t;

	typedef enum MqTestVal
	{
		MqAllOff,
		MqAllOn,
		MqCheckerBoard,
		MqCharacter
	}   MqTestVal_t;

	typedef enum MqBufferType
	{
		MqProduct,
		MqDaily,
		MqPermanent
	}   MqBufferType_t;

	typedef enum MqOperationMode
	{
		MqOpModeNormal,
		MqOpModeOutOfServ

	}   MqOperationMode_t;


#ifdef __cplusplus
	#pragma pack()
#endif

#endif //MARQUEE_H
