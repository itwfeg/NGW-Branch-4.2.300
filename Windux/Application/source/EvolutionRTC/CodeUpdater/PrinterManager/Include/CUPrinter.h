
#ifndef CUPRINTER_H
#define CUPRINTER_H


#ifdef __cplusplus
	#pragma pack(1)
#endif

	// Different Kinds of Numerics
	typedef enum CuPrnCmdEnum
	{
		CuPrnRunNormal,
		CuPrnBurnPrinter,
		CuPrnBurnBootCode,
		CuPrnBurnFlud,
		CuPrnBurnAtAddress,
		CuPrnResetErrCounter
	} CuPrnCmdEnum_t;


#ifdef __cplusplus
	#pragma pack()
#endif

#endif //CUPRINTER_H
